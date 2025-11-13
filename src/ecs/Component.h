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

struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    float width{};
    float height{};
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
    SDL_Texture* texture = nullptr;
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
    std::unordered_map<std::string, AnimationClip> clips {};
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
    float timer {};
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
    int gunType = 1;
    bool shooting = false;
    float fireRatePerSecond = 1.0f;
};

struct Boost {
    bool boosting = false;
    float boostTimeoutSeconds = 1;
    float boostYVelocity = 100; // 2 times gravity (9.81*32)=(gravity*pixels)
};

// Physics.velocity = actual x/y velocity applied to transform, including gravity, drag, and boost
struct Physics {
    const float gravityMetersPerSecond = 9.81; // m/s -  we assume that 1 tile is 1 meter
    const float gravityPixelsPerSecond2 = gravityMetersPerSecond * 32; //32 being tile size = 313.92
    const float maxSpeed = 100.0f;
    float height = 0.0f; // how far above groundY
    Vector2D velocity{};
    bool grounded = false;

    float airDrag = 0.05f; // 5% / second in air
    float groundFriction = 0.20f;  // % / per second on ground - 0, but might apply to obstacles
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
    // Optional: use a texture, or just use a default black ellipse
    SDL_Texture* texture = nullptr;
};

struct HitBox {
    // Base (reference) hitboxes
    float hitBox1baseOffsetX = 0, hitBox1baseOffsetY = 0, hitBox1baseWidth = 0, hitBox1baseHeight = 0;
    float hitBox2baseOffsetX = 0, hitBox2baseOffsetY = 0, hitBox2baseWidth = 0, hitBox2baseHeight = 0;

    // Active hitboxes (scaled/modified)
    float hitBox1offsetX = 0, hitBox1offsetY = 0, hitBox1width = 0, hitBox1height = 0;
    float hitBox2offsetX = 0, hitBox2offsetY = 0, hitBox2width = 0, hitBox2height = 0;

    // Just so I can initialize both with same values
    HitBox(float hb1x, float hb1y, float hb1w, float hb1h,
           float hb2x, float hb2y, float hb2w, float hb2h)
    {
        // Initialize both base and current at once
        hitBox1baseOffsetX = hitBox1offsetX = hb1x;
        hitBox1baseOffsetY = hitBox1offsetY = hb1y;
        hitBox1baseWidth   = hitBox1width   = hb1w;
        hitBox1baseHeight  = hitBox1height  = hb1h;

        hitBox2baseOffsetX = hitBox2offsetX = hb2x;
        hitBox2baseOffsetY = hitBox2offsetY = hb2y;
        hitBox2baseWidth   = hitBox2width   = hb2w;
        hitBox2baseHeight  = hitBox2height  = hb2h;
    }
};


//TAG INDICATORS
struct PlayerTag{}; // can just look for player tag for camera follow
struct ProjectileTag{};


#endif //COMP8552_FINAL_PROJECT_COMPONENT_H