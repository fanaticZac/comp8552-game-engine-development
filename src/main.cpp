#include <iostream>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>

#include "Game.h"

// global variable
Game *game = nullptr;

int main() {
    // HARDCODE PROJECT SETTINGS
    const int FPS = 60;
    const int desiredFrameRate = 1000 / FPS; //16ms
    float deltaTime = 0.0f;


    Uint64 ticks = 0;
    int actualFrameTime;

    game = new Game();

    // 1280x720 = 16:9 better for mobile conversion
    game->init("Epoch Apocalypse: Merchant Soldiers", 1280, 736, false);

    // GAME LOOP - CORE
    while (game->running()) {

        int currentTicks = SDL_GetTicks(); // time in ms since game start
        deltaTime = (currentTicks - ticks) / 1000.0f; //ticks per second
        ticks = currentTicks;

        // GAME LOOP - CORE FUNCTIONS
        game->handleEvents();
        game->update(deltaTime);
        game->render();

        actualFrameTime = SDL_GetTicks() - ticks; //elaspsed time in ms it took the current frame;



        // Keeps game running at the desired frame rate
        //if the actual frame took less time then the desired frame, delay the difference
        if (desiredFrameRate > actualFrameTime) {
            SDL_Delay(desiredFrameRate - actualFrameTime);
        }


    }
    delete game;
    return 0;
}
