//
// Created by user on 2025-09-16.
//

#ifndef COMP8552_FINAL_PROJECT_COMPONENT_H
#define COMP8552_FINAL_PROJECT_COMPONENT_H

#include <functional>
#include <SDL3/SDL_render.h>

#include "../utils/Vector2D.h"
#include <string>
#include <unordered_map>

#include "AnimationClip.h"
#include "Entity.h"

struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    float width{};
    float height{};
    float depthWidth{}; // for use with groundY hitboxes (probably same as width is entity is cylindrical)
    Vector2D oldPosition{};
    // Tried using Vector3D but it got confusing...
    // We really just want to track the existing groundY
    // aka where the player would fall to if not flying
    // So when player moves this is same as position.y (without phsyics)
    // but only changes when W/S is pressed - not when boost
    // This will also be used for alignment for collisions in 2.5D
    float groundY = 400;
    float oldGroundY = 400;
};

// Velocity = player input intent (direction + speed)
struct Velocity {
    Vector2D direction{};
    float speed{};
};

struct Sprite {
    SDL_Texture *texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};

    // Added
    float baseWidth = 192;
    float baseHeight = 192;
    float maxScale = 1.0f;
    float minScale = 0.707;
    float currentScale = 1.0f;
};

struct DebugSprite { // For hitbox visual debugging system
    SDL_Texture *texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};

    // Added
    float baseWidth = 192;
    float baseHeight = 192;
    float maxScale = 1.0f;
    float minScale = 0.707;
    float currentScale = 1.0f;
};

struct Collider {
    std::string tag;
    SDL_FRect rect{};
};

struct Animation {
    std::unordered_map<std::string, AnimationClip> clips{};
    std::string currentClip{};
    float time{}; //time accumulated for the current frame
    int currentFrame{}; //index of the current frame in the clip
    float speed = 0.1f; // time per frame
};

struct Camera {
    SDL_FRect view;
    float worldWidth;
    float worldHeight;
};

struct TimedSpawner {
    float spawnInterval{};
    std::function<void()> spawnCallback{};
    float timer{};
};

// our game state, might have multiple scenes
struct SceneState {
    int coinsCollected = 0;
};

struct Health {
    int currentHealth{};
};

// Added -
struct Gun {
    // Distance scaling woes
    // OFFSET 1 IS
    float gunTip1BaseOffsetX = 0.0f, gunTip1BaseOffsetY = 0.0f, gunTip2BaseOffsetX = 0.0f, gunTip2BaseOffsetY = 0.0f;
    float gunTip1OffsetXScaled = 0.0f, gunTip1OffsetYScaled = 0.0f, gunTip2OffsetXScaled = 0.0f, gunTip2OffsetYScaled =
                    0.0f;

    bool gun1Shooting = false;
    float gun1FireRatePerSecond = 1.0f;
    bool gun2Shooting = false;
    float gun2FireRatePerSecond = 1.0f;

    Gun(float gt1x, float gt1y, float gt2x, float gt2y) {
        gunTip1BaseOffsetX = gunTip1OffsetXScaled = gt1x;
        gunTip1BaseOffsetY = gunTip1OffsetYScaled = gt1y;

        gunTip2BaseOffsetX = gunTip2OffsetXScaled = gt2x;
        gunTip2BaseOffsetY = gunTip2OffsetYScaled = gt2y;
    }
};

struct Boost {
    bool boosting = false;
    float boostTimeoutSeconds = 1;
    float boostYVelocity = 120; // (9.81*32)=(gravity*pixels)
};

// Physics.velocity = actual x/y velocity applied to transform, including gravity, drag, and boost
struct Physics {
    const float gravityMetersPerSecond = 9.81; // m/s -  we assume that 1 tile is 1 meter
    const float gravityPixelsPerSecond2 = gravityMetersPerSecond * 32; //32 being tile size = 313.92
    const float xMaxSpeed = 180.0f;
    float height = 0.0f; // how far above groundY
    Vector2D velocity{};
    bool grounded = false;

    float airDrag = 0.05f; // 5% / second in air
    float groundFriction = 0.20f; // % / per second on ground - 0, but might apply to obstacles
};

struct Input {
    bool moveTop = false;
    bool moveRight = false;
    bool moveBottom = false;
    bool moveLeft = false;
    bool boost = false;
    bool shootGun1 = false;
    bool shootGun2 = false;
};

struct Shadow {
    float maxScale = 1.0f;
    float minScale = 0.3f;
    float currentScale = 1.0f;
    Uint8 currentAlpha = 255;
    float renderX = 0;
    float renderY = 0;
    SDL_Texture *texture = nullptr;
};

struct HitBox {
    // Base (reference) hitboxes
    float hitBox1baseOffsetX = 0, hitBox1baseOffsetY = 0, hitBox1baseWidth = 0, hitBox1baseHeight = 0;
    float hitBox2baseOffsetX = 0, hitBox2baseOffsetY = 0, hitBox2baseWidth = 0, hitBox2baseHeight = 0;

    // Active hitboxes (scaled/modified)
    // HITBOX 2 IS FOR ON GROUND
    float hitBox1offsetXScaled = 0, hitBox1offsetYScaled = 0, hitBox1widthScaled = 0, hitBox1heightScaled = 0;
    // HITBOX 2 IS FOR BOOSTING (SLIGHT HEIGHT INCREASE)
    float hitBox2offsetXScaled = 0, hitBox2offsetYScaled = 0, hitBox2widthScaled = 0, hitBox2heightScaled = 0;

    // Just so I can initialize both with same values
    HitBox(float hb1x, float hb1y, float hb1w, float hb1h,
           float hb2x, float hb2y, float hb2w, float hb2h) {
        // Initialize both base and current at once
        hitBox1baseOffsetX = hitBox1offsetXScaled = hb1x;
        hitBox1baseOffsetY = hitBox1offsetYScaled = hb1y;
        hitBox1baseWidth = hitBox1widthScaled = hb1w;
        hitBox1baseHeight = hitBox1heightScaled = hb1h;

        hitBox2baseOffsetX = hitBox2offsetXScaled = hb2x;
        hitBox2baseOffsetY = hitBox2offsetYScaled = hb2y;
        hitBox2baseWidth = hitBox2widthScaled = hb2w;
        hitBox2baseHeight = hitBox2heightScaled = hb2h;
    }
};

struct Projectile {
    float speed = 360.0f; // pixels/sec
    int damage = 1;
    float lifetime = 2.0f;
    int ownerID = -1;
    bool piercing;
};

struct EnemyBehavior {
    enum class State {
        Idle,
        Charging,
        Shooting,
        Dying
    };

    State fireState = State::Idle;

    // For random firing
    float fireTimer = 0.0f;
    float nextFireTime = 1.0f;
    float stateTimer = 0.0f;

    // Movement
    float movementSpeedGroundY = 180.0f;
    float movementSpeedX = 180.0f;
    float movementSpeedY = 180.0f;

    // Direction
    int directionX = 1;
    int directionY = 1;
    int directionGround = 1;

    float moveTimer = 0.0f;
    float nextMoveSwitchTime = 1.0f;

    // Actual booleans
    bool dying = false;
    bool charging = false;
    bool shooting = false;
};

struct Enemy { // replaces tag
    int playerCollisionDamage = 1; // aka direct player/enemy collision
};

//TAG INDICATORS
struct PlayerTag{}; // can just look for player tag for camera follow
struct ScaleThisObjectByDistance {}; // Just enemies/player (shots are scaled in shootingSyste)


#endif //COMP8552_FINAL_PROJECT_COMPONENT_H