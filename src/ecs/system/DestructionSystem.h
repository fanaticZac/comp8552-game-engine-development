//
// Created by user on 2025-10-21.
//

#ifndef COMP8552_FINAL_PROJECT_DESTRUCTIONSYSTEM_H
#define COMP8552_FINAL_PROJECT_DESTRUCTIONSYSTEM_H

#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class DestructionSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity> > &entities) {
        Entity *cameraEntity = nullptr;
        // find camera
        for (auto &e: entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }

        if (!cameraEntity) return;
        auto &cam = cameraEntity->getComponent<Camera>();

        for (auto &e: entities) {
            if (e->hasComponent<Transform>() && e->hasComponent<Projectile>()) {
                auto &t = e->getComponent<Transform>();
                // destroy entity if goes out of cam view
                if (
                    t.position.x > cam.view.x + cam.view.w || // right edge
                    t.position.x < cam.view.x || //left
                    t.position.y > cam.view.y + cam.view.h || //bottom
                    t.position.y < cam.view.y) { //top
                    e->destroy();
                }
            }
        }
    }
};

#endif //COMP8552_FINAL_PROJECT_DESTRUCTIONSYSTEM_H