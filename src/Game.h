//
// Created by user on 2025-09-02.
//

#ifndef COMP8552_FINAL_PROJECT_GAME_H
#define COMP8552_FINAL_PROJECT_GAME_H

#pragma once

#include <functional>
#include <string>
#include <SDL3/SDL.h>

#include "CollisionSystem.h"
#include "manager/SceneManager.h"
#include "World.h"


// NOTE: https://wiki.libsdl.org/SDL3/CategoryAPI
// NOTE: https://wiki.libsdl.org/SDL3/QuickReference

struct GameState {
    int playerHealth;
};

class Game {
public:
    Game();
    ~Game();

    // These are the types SDL uses
    // SDL = Simple Direct-Media Layer
    // --- Access to low level graphics / hardware / system
    // --- Most (non-top-tier-professional) game engines will use 3rd party system like this.
    // --- MONOGAME - is another - using C# and .NET
    // --- SFML, GFLW, Allegro, RayLib
    void init(const char* title, int width, int height, bool fullscreen);

    // GAME LOOP FUNCTIONS - FUNDAMENTAL!!!
    // handleEvents, update, render
    void handleEvents(); // Checks for input and system events
    void update(float deltaTime); // Handles logic and changes to game state
    void render(); // Handles the drawing of the current game state to the screen
    // These events will run every single frame
    // Basically at its core - what a game is.

    // Used to free resources
    void destroy();

    bool running() const;

    SDL_Renderer* renderer = nullptr;

    SceneManager sceneManager;
    static std::function<void(std::string)> onSceneChangeRequest; // when we change a scene, it will go through game
    static GameState gameState;

    World world;
private:
    int frameCount = 0;
    bool isRunning;

    SDL_Window* window;
    SDL_Event event;
    // SDL stores color channels and 8-bit unsigned 0-255 ranges
    Uint8 r,g,b,a;

};

#endif //COMP8552_FINAL_PROJECT_GAME_H