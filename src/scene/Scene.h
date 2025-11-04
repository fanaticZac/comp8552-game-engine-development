//
// Created by user on 2025-10-21.
//

#ifndef COMP8552_FINAL_PROJECT_SCENE_H
#define COMP8552_FINAL_PROJECT_SCENE_H

#include <string>
#include <SDL3/SDL_events.h>
#include "World.h"
#include "Vector2D.h"
#include "SceneType.h"

// We're moving the responsibility of entity creation to scene from game.
class Scene {
public:
    Scene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight);

    void update(const float dt, const SDL_Event &e) {
        world.update(dt, e, type);
    }

    void render() {
        world.render();
    }

    // Every new scene will have its own world.
    World world;

    const std::string& getName () const { return name;}


private:
    std::string name;
    void createProjectile(Vector2D pos, Vector2D dir, int speed);
    SceneType type;
};

#endif //COMP8552_FINAL_PROJECT_SCENE_H