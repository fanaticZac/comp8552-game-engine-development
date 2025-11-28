//
// Created by user on 2025-11-27.
//

#ifndef COMP8552_FINAL_PROJECT_HEALTHSYSTEM_H
#define COMP8552_FINAL_PROJECT_HEALTHSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class HealthSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>> &entities, float dt) {
        for (auto &entity : entities) {
            if (!entity->hasComponent<Health>()) continue;

            auto &health = entity->getComponent<Health>();

            if (health.damageFlash) {
                health.damageFlashTimer -= dt;
                if (health.damageFlashTimer <= 0.0f) {
                    health.damageFlash = false;
                    health.damageFlashTimer = 0.0f;
                }
            }
        }
    }
};

#endif //COMP8552_FINAL_PROJECT_HEALTHSYSTEM_H