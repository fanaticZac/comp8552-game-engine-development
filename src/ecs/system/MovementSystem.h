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
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& entity: entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
                auto& t = entity->getComponent<Transform>();
                auto& v = entity->getComponent<Velocity>();
                // t.position.x += 60 * dt;
                // t.position.y += 60 * dt;

                // Track previous frames position
                t.oldPosition = t.position;


                Vector2D directionVec = v.direction;
                // Normalizing hasn't happened yet
                // Vector2D needs an operator function to multiply a float to itself
                // Example using both operator function orders, check Vector2D function
                // Vector2D velocityVec1 = directionVec * v.speed;
                // Vector2D velocityVec2 = v.speed * directionVec;

                //Normalize so that our vectors are just direction and not magnitude
                directionVec.normalize();

                Vector2D velocityVec = directionVec * v.speed;
                t.position += (velocityVec * dt);




            }
        }
    }

};

#endif //COMP8552_FINAL_PROJECT_MOVEMENTSYSTEM_H