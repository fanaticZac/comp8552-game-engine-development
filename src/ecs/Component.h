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
    Vector2D oldPosition{};
};

// Direction and speed
struct Velocity {
    Vector2D direction{};
    float speed{};
};

// struct Position {
//     float x = 0.0f;
//     float y = 0.0f;
// };

struct Sprite {
    SDL_Texture* texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
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

//TAG INDICATORS
struct PlayerTag{}; // can just look for player tag for camera follow
struct ProjectileTag{};


#endif //COMP8552_FINAL_PROJECT_COMPONENT_H