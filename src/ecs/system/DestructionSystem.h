//
// Created by user on 2025-10-21.
//

#ifndef COMP8552_FINAL_PROJECT_DESTRUCTIONSYSTEM_H
#define COMP8552_FINAL_PROJECT_DESTRUCTIONSYSTEM_H

#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "Map.h"

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
            if (e->hasComponent<Transform>()) {
                auto &t = e->getComponent<Transform>();
                if (e->hasComponent<Projectile>()) {
                    // destroy entity if goes out of cam view
                    // if (
                    //     t.position.x > cam.view.x + cam.view.w || // right edge
                    //     t.position.x < cam.view.x || //left
                    //     t.position.y > cam.view.y + cam.view.h || //bottom
                    //     t.position.y < cam.view.y) {
                    //     //top
                    //     std::cout << "Entity Destroyed: " << e->getID() << std::endl;
                    //     e->destroy();
                    // }

                    // maps not that big on this - we will use width paths edge of screen to avoid disappearing early or when moving back in.
                    if (
                        t.position.x > Map::screenRight + t.width || // right edge
                        t.position.x < Map::screenLeft - t.width  || //left
                        t.position.y > Map::screenBottom + t.height || //bottom
                        t.position.y < Map::screenTop - t.height) {
                        //top
                        // std::cout << "Entity Destroyed: " << e->getID() << std::endl;
                        e->destroy();
                    }
                }

                // obstacle just deletes things off right hand side
                // - things will be generated past right hand screen side and move left always.
                if (e->hasComponent<Obstacle>() && t.position.x + t.width < 0) {
                    // std::cout << "Obstacle Destroyed: " << e->getID() << "Tranform.x: " <<  t.position.x << std::endl;
                    e->destroy();
                }
            }
        }
    }
};

#endif //COMP8552_FINAL_PROJECT_DESTRUCTIONSYSTEM_H
