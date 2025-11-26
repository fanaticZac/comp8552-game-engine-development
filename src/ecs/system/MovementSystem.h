//
// Created by user on 2025-09-16.
//

#ifndef COMP8552_FINAL_PROJECT_MOVEMENTSYSTEM_H
#define COMP8552_FINAL_PROJECT_MOVEMENTSYSTEM_H

#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"


class MovementSystem {
public:
    void update(std::vector<std::unique_ptr<Entity> > &entities, float dt) {
        for (auto &entity: entities) {
            if (entity->hasComponent<Velocity>() &&
                entity->hasComponent<Transform>()) {
                auto &velocity = entity->getComponent<Velocity>();
                auto &transform = entity->getComponent<Transform>();

                if (!entity->hasComponent<Player>() && !entity->hasComponent<Physics>()) {
                    Vector2D dir = velocity.direction;
                    dir.normalize();
                    Vector2D velVec = dir * velocity.speed;

                    transform.oldPosition = transform.position;
                    transform.position += velVec * dt;

                }
            }
        }
    }

private:

};

#endif //COMP8552_FINAL_PROJECT_MOVEMENTSYSTEM_H
