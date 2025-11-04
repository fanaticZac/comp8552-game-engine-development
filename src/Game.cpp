//
// Created by user on 2025-09-02.
//

#include "Game.h"
#include <iostream>

#include "AssetManager.h"

// NOTE: https://wiki.libsdl.org/SDL3/CategoryAPI
// NOTE: https://wiki.libsdl.org/SDL3/QuickReference


GameState Game::gameState;
std::function<void(std::string)> Game::onSceneChangeRequest;


// BUILT-IN FUNCTIONS ----- START

Game::Game() {

}

Game::~Game() {
    // Clean up
    destroy();
}

// BUILT-IN FUNCTIONS ----- END
// SYSTEM FUNCTIONS ----- START

void Game::init(const char *title, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    // Initialize SDL Library ! for now - just the window
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 1) {
        std::cout << "Subsystem Initialized" << std::endl;

        // Create window
        // NOTE: https://wiki.libsdl.org/SDL3/SDL_CreateWindow
        window = SDL_CreateWindow(title, width, height, flags);
        if (window) {
            std::cout << "Window created!" << std::endl;
        }else {
            std::cout << "Window not created." << std::endl;

        }

        // Create Renderer
        // NOTE: https://wiki.libsdl.org/SDL3/SDL_CreateRenderer
        // OpenGL most likily for Linux, via Direct3D for windows perhaps
        renderer = SDL_CreateRenderer(window, nullptr);
        if (renderer) {
            std::cout << "Renderer Created!" << std::endl;
        } else {
            std::cout << "Window not created." << std::endl;
            return;
        }

        isRunning = true;
    }else {
        isRunning = false;
    }

    //Load assets
    AssetManager::loadAnimation("player", "../assets/animations/sheep_animations.xml");
    AssetManager::loadAnimation("enemy", "../assets/animations/bird_animations.xml");

    //Load Scenes
    sceneManager.loadScene(SceneType::MainMenu, "mainmenu", nullptr, width, height);
    sceneManager.loadScene(SceneType::Gameplay, "level1", "../assets/map.tmx", width, height);
    sceneManager.loadScene(SceneType::Gameplay, "level2", "../assets/map2.tmx", width, height);

    // Init Game State
    gameState.playerHealth = 5;

    // Load initial
    sceneManager.changeSceneDeferred("mainmenu");

    // Resolve scene callback
    onSceneChangeRequest = [this](std::string sceneName) {
        // Some game state happening, not changes to world
        // This is just high level game state stuff

        if (sceneManager.currentScene->getName() == "level2" && sceneName == "level2") {
            std::cout << "You win" << std::endl;
            isRunning = false;
            return;
        }

        if (sceneName == "gameover") {
            isRunning = false;
            return;
        }

        sceneManager.changeSceneDeferred(sceneName);
    };
}

void Game::destroy() {
    TextureManager::clean();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "GAME DESTROYED!" << std::endl;
}


// SYSTEM FUNCTIONS ----- END
// GAME LOOP ----- START

void Game::handleEvents() {
    // SDL listens to the OS for input events internally and adds them to a queue

    // SDL_Event event;
    // Check for next event, if an event is available it will remove from the queue and store in event.
    // NOTE: https://wiki.libsdl.org/SDL3/SDL_PollEvent
    SDL_PollEvent(&event);

    // TODO: Go Through SDL options for this and analyse potential additions
    switch (event.type) {
        case SDL_EVENT_QUIT: // Usually triggered when use closes window.
            isRunning = false;
            break;
        default:
            break;
    }


}

void Game::update(float deltaTime) {
    sceneManager.update(deltaTime, event);
}

void Game::render() {
    r = 100;
    g = 255;
    b = 50;
    a = 255;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    // with the drawing color
    SDL_RenderClear(renderer);

    sceneManager.render();

    // Draw map
    // map->draw();

    // drap player
    // player->draw();
    // world.render();

    //all your drawing would go here
    //draws it in a memory first to a back buffer
    SDL_RenderPresent(renderer);
    // important to clear and then draw - causes flickering otherwise
}


// GAME LOOP FUNCTIONS ----- END
// GETTER/SETTERS ----- START
s
bool Game::running() const {
    return isRunning;
}

// GETTER/SETTERS ----- END

