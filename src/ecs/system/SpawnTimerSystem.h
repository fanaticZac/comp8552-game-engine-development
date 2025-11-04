//
// Created by user on 2025-10-21.
//

#ifndef COMP8552_FINAL_PROJECT_SPAWNTIMERSYSTEM_H
#define COMP8552_FINAL_PROJECT_SPAWNTIMERSYSTEM_H

#include <memory>
#include <vector>

#include "Entity.h"
#include "Component.h"

class SpawnTimerSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity: entities) {
            if (entity->hasComponent<TimedSpawner>()) {
                auto& spawner = entity->getComponent<TimedSpawner>();

                spawner.timer -= dt;
                if (spawner.timer <= 0) {
                    spawner.timer = spawner.spawnInterval; // e.g. 2 seconds
                    spawner.spawnCallback(); // create entity inside here
                }
            }
        }
    }
};

#endif //COMP8552_FINAL_PROJECT_SPAWNTIMERSYSTEM_H