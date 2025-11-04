//
// Created by user on 2025-10-21.
//

#ifndef COMP8552_FINAL_PROJECT_SCENEMANAGER_H
#define COMP8552_FINAL_PROJECT_SCENEMANAGER_H

#include <iostream>
#include <string>
#include <unordered_map>

#include "../scene/Scene.h"

struct SceneParams {
    SceneType sceneType;
    const char *name;
    const char *mapPath;
    int windowWidth;
    int windowheight;
};

class SceneManager {
    std::unordered_map<std::string, SceneParams> sceneParams;
    std::string pendingScene;

    void changeScene(const std::string &name) { // e.g. level-1, level-2
        auto it = sceneParams.find(name);
        if (it != sceneParams.end()) {
            const auto &params = it->second;

            currentScene = std::make_unique<Scene>(
                params.sceneType,
                params.name,
                params.mapPath,
                params.windowWidth,
                params.windowheight);
        } else {
            std::cout << "Scene name: " << name << "not found!" << std::endl;
        }
    }

public:
    std::unique_ptr<Scene> currentScene;

    void loadScene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight) {
        sceneParams[sceneName] = {sceneType, sceneName, mapPath, windowWidth, windowHeight};
    }

    void changeSceneDeferred(const std::string& sceneName) {
        pendingScene = sceneName;
    }

    void update(const float dt, const SDL_Event &e) {
        if (currentScene) currentScene->update(dt, e); // all our world updates
        if (!pendingScene.empty()) {
            changeScene(pendingScene);
            pendingScene.clear();
        }
    }

    void render () {
        if (currentScene) currentScene->render();
    }
};


#endif //COMP8552_FINAL_PROJECT_SCENEMANAGER_H