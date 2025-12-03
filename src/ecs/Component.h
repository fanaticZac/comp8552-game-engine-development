//
// Created by user on 2025-09-16.
//

#ifndef COMP8552_FINAL_PROJECT_COMPONENT_H
#define COMP8552_FINAL_PROJECT_COMPONENT_H

#include <cmath>
#include <functional>
#include <iostream>
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
    float depth{};
    Vector2D oldPosition{};
    // Tried using Vector3D but it got confusing...
    // We really just want to track the existing groundY
    // aka where the player would fall to if not flying
    // So when player moves this is same as position.y (without phsyics)
    // but only changes when W/S is pressed - not when boost
    // This will also be used for alignment for collisions in 2.5D
    float groundZ = 400;
    float oldGroundZ = 400;
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

struct DebugSprites {
    std::vector<Sprite> sprites;
};

struct Collider {
    std::string tag;
    // For player: HB1 is grounded, HB2 is in flight
    // For obstacles (rectangular prisms) HB1 is front square, HB2 is back square - and we interpolate
    // HB3 is for obstacles - to create a dynamic HB interpolated between HB1 and HB2 based on GroundZ values of colliding entities
    SDL_FRect hitBox1base{};
    SDL_FRect hitBox1Scaled{};
    SDL_FRect hitBox2base{};
    SDL_FRect hitBox2Scaled{};
    bool hitbox3_isActive = false;
    SDL_FRect hitBox3base{};
    SDL_FRect hitBox3Scaled{};

    Collider() = default;

    Collider(const std::string &collisionTag, const SDL_FRect hitBox1) {
        // Initialize both base and current at once
        tag = collisionTag;
        hitBox1base = hitBox1Scaled = hitBox1;
        hitBox2base = hitBox2Scaled = hitBox1;
    }

    Collider(const std::string &collisionTag, const SDL_FRect hitBox1, const SDL_FRect hitBox2) {
        // Initialize both base and current at once
        tag = collisionTag;
        hitBox1base = hitBox1Scaled = hitBox1;
        hitBox2base = hitBox2Scaled = hitBox2;
    }

    bool createDynamicHitBox3ForObstacles(float otherGroundZ, float wallGroundZ) {
        // Determine absolute depth between HB1 and HB2 in X
        float cubeDepthMaximum =
            std::fabs(hitBox1Scaled.x - hitBox2Scaled.x);

        if (cubeDepthMaximum == 0.0f)
            return false;

        // How far "into the cube" the other object is
        float cubeDepthGroundZ = wallGroundZ - otherGroundZ;

        float groundZ_alpha = cubeDepthGroundZ / cubeDepthMaximum;
        groundZ_alpha = std::clamp(groundZ_alpha, 0.0f, 1.0f);

        float newCubeHitBoxOffsetValue = cubeDepthMaximum * (1.0f - groundZ_alpha);

        // Activate HB3 and apply interpolated values
        hitbox3_isActive = true;

        hitBox3Scaled.x = newCubeHitBoxOffsetValue;
        hitBox3Scaled.y = newCubeHitBoxOffsetValue;

        hitBox3Scaled.w = hitBox2Scaled.w;
        hitBox3Scaled.h = hitBox2Scaled.h;

        // Debug
        // std::cout << "Collider::createDynamicHitBox3\n";
        // std::cout << "cubeDepthMaximum = " << cubeDepthMaximum << "\n";
        // std::cout << "cubeDepthGroundZ = " << cubeDepthGroundZ << "\n";
        // std::cout << "groundZ_alpha = " << groundZ_alpha << "\n";
        // std::cout << "newCubeHitBoxOffsetValue = " << newCubeHitBoxOffsetValue << "\n";

        return true;
    }
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
    bool damageFlash = false;
    float damageFlashTimer = 0.2f;
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
    const float xMaxSpeed = 360.0f;
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
    bool moveBackwards = false;
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


struct Projectile {
    float speed = 1000.0f; // pixels/sec
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
    float movementSpeedGroundY = 300.0f;
    float movementSpeedX = 300.0f;
    float movementSpeedY = 300.0f;

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

//TAG INDICATORS2
struct Player {
    bool isStandingOnObstacle = false;
    int isStandingBehindObstacleEntityID = -1;
}; // can just look for player tag for camera follow

struct Obstacle {

}; // For destruction system to properly delete when leaving camera view.

struct CarObstacle {};
struct ScaleThisObjectByDistance {}; // Just enemies/player (shots are scaled in shootingSyste)


#endif //COMP8552_FINAL_PROJECT_COMPONENT_H