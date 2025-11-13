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

                // FOR CLARIFICATION FOR MYSELF about ECS:
                // INPUTS -> MOVEMENT SYSTEM -> PHYSICS SYSTEM -> COLLIDER SYSTEM -> RENDER SYSTEM
                // INPUTS TAKE RAW INPUT, ADD TO COMPONENT
                // MOVEMENT SYSTEM TRANSLATES INPUTS TO PHYSICAL MOTION (VELOCITY COMPONENT) - READS INPUTS PHYSICS, WRITES VELOCITY, BOOST
                // PHYSICS APPLIES PHYSICS ON IT (PHYSICS COMPONENT) and apply this to transform - READS VELOCITY, BOOST, PHYSICS, WRITES TRANFORM VELOCITY
                // COLLIDER SYSTEM (IS WHERE ILL SET MY LEVEL BOUNDARY FOR groundY)

                // In MovementSystem, you only update Velocity.direction
                // In PhysicsSystem, you apply it to Physics.velocity, then integrate to Transform.position.

                // Track previous frames position
                transform.oldPosition = transform.position;
                // transform.oldGroundY = transform.groundY;

                if (!entity->hasComponent<PlayerTag>() && !entity->hasComponent<Physics>()) {
                    Vector2D directionVec = velocity.direction;
                    // Normalizing hasn't happened yet
                    // Vector2D needs an operator function to multiply a float to itself
                    // Example using both operator function orders, check Vector2D function
                    // Vector2D velocityVec1 = directionVec * v.speed;
                    // Vector2D velocityVec2 = v.speed * directionVec;

                    //Normalize so that our vectors are just direction and not magnitude
                    directionVec.normalize();

                    Vector2D velocityVec = directionVec * velocity.speed;
                    transform.position += (velocityVec * dt);
                }
            }
        }
    }
};

#endif //COMP8552_FINAL_PROJECT_MOVEMENTSYSTEM_H
