//
// Created by user on 2025-11-04.
//

#include "MainMenuSystem.h"

#include "Game.h"

void MainMenuSystem::update(const SDL_Event &event) {

    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_SPACE:
                Game::onSceneChangeRequest("level1");
                break;
            default:
                break;
        }
    }
}
