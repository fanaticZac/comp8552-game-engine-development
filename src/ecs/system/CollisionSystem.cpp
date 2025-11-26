//
// Created by user on 2025-10-04.
//

#include "CollisionSystem.h"

#include <cmath>
#include <cstring>
#include <iostream>

#include "Collision.h"
#include "World.h"

// Has 2 important functions
// - position collider with transform
// - checking for collisions
// Note: this can be improved for effeciency
void CollisionSystem::update(World &world) {
    // Get a list of entities that have transforms and colliders
    const std::vector<Entity *> collidables = queryCollidables(world.getEntities());

    //update all collider positions first
    // for (auto entity: collidables) {
    //     auto &t = entity->getComponent<Transform>();
    //     auto &c = entity->getComponent<Collider>();
    //     //sect collider to match position
    //     c.hitBox1base.x = t.position.x;
    //     c.hitBox1base.y = t.position.y;
    // }

    std::set<CollisionKey> currentCollisions;

    // Handle collisions
    // Outer loop
    for (size_t i = 0; i < collidables.size(); i++) {
        //update the collider position
        auto entityA = collidables[i];
        auto &tA = entityA->getComponent<Transform>();
        auto &cA = entityA->getComponent<Collider>();

        SDL_FRect hbA;
        if (entityA->hasComponent<Player>() && !entityA->getComponent<Physics>().grounded) {
            hbA = {
                tA.position.x + cA.hitBox2Scaled.x,
                tA.position.y + cA.hitBox2Scaled.y,
                cA.hitBox2Scaled.w,
                cA.hitBox2Scaled.h
            };
        } else if (entityA->hasComponent<Obstacle>()) {
            // This will be adjusted later.
            hbA = {
                tA.position.x  + cA.hitBox2Scaled.x,
                tA.position.y  + cA.hitBox2Scaled.y,
                cA.hitBox1Scaled.w,
                cA.hitBox1Scaled.h
            };
        } else {
            // Most 2D objects - like projectiles, use the first hitbox

            hbA = {
                tA.position.x + cA.hitBox1Scaled.x,
                tA.position.y + cA.hitBox1Scaled.y,
                cA.hitBox1Scaled.w,
                cA.hitBox1Scaled.h
            };
        }

        // Inner loop
        // Check for collisions
        for (size_t j = i + 1; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto &cB = entityB->getComponent<Collider>();
            auto &tB = entityB->getComponent<Transform>();

            if (entityA->hasComponent<Obstacle>() && entityB->hasComponent<Obstacle>()) {
                continue;
            }

            // NOTE: Here we make adjustment to use entities hitbox and ensure they are both within the groundY depth range
            // NOTE: -- to qualify a collision for this system
            // This checks the groundZ value against the depthWidth threshold aka how wide Z-index is in the 2.5 dimension
            // effect the back of object if shorter than image aka depth from front is value.
            // // Ensure horizontal alignment (depth is used differently for 3D obstacles)
            if (!entityA->hasComponent<Obstacle>() && !entityB->hasComponent<Obstacle>()) {
                const float depthDist = std::fabs(tA.groundZ - tB.groundZ);
                const float depthThreshold = (tA.depth + tB.depth) * 0.5f;
                if (depthDist > depthThreshold) {
                    continue;
                }
            }

            if (entityA->hasComponent<Obstacle>()) {
                if (tB.groundZ > tA.groundZ) {
                    // std::cout << "COLLISION ERROR: player in front cube ";
                    continue;
                }
                if (tB.groundZ < tA.groundZ - tA.height + tA.depth) {
                    // std::cout << "COLLISION ERROR: player behind of cube ";
                    continue;
                }

                // NOTE: This is very important for the dynamic hitbox, for use in the EventResponseSystem
                cA.hitbox3_isActive = cA.createDynamicHitBox3ForObstacles(tB.groundZ, tA.groundZ);
                hbA = {
                    tA.position.x  + cA.hitBox3Scaled.x,
                    tA.position.y  + cA.hitBox3Scaled.y,
                    cA.hitBox3Scaled.w,
                    cA.hitBox3Scaled.h
                };


            }



            // Use HitBoxes
            SDL_FRect hbB;
            if (entityB->hasComponent<Player>() && !entityB->getComponent<Physics>().grounded) {
                hbB = {
                    tB.position.x + cB.hitBox2Scaled.x,
                    tB.position.y + cB.hitBox2Scaled.y,
                    cB.hitBox2Scaled.w,
                    cB.hitBox2Scaled.h
                };
            } else if (entityB->hasComponent<Obstacle>()) {
                // Just make hitBox entire image - this will be handled in EventSystem for wall
                // NOTE: This is very important for the dynamic hitbox, for use in the EventResponseSystem
                if (tA.groundZ > tB.groundZ) {
                    // std::cout << "COLLISION ERROR: player  in front  cube ";
                    continue;
                }
                if (tA.groundZ < tB.groundZ - tB.height + tB.depth) {
                    // std::cout << "COLLISION ERROR: player behindof cube ";
                    continue;
                }

                cB.hitbox3_isActive = cB.createDynamicHitBox3ForObstacles(tA.groundZ, tB.groundZ);
                hbB = {
                    tB.position.x  + cB.hitBox3Scaled.x,
                    tB.position.y  + cB.hitBox3Scaled.y,
                    cB.hitBox3Scaled.w,
                    cB.hitBox3Scaled.h
                };

                // Depth compare with groundZ done in EventResponse - we just want overlap here for obstacles
            } else {
                // Most 2D objects - like projectiles, use the first hitbox
                hbB = {
                    tB.position.x + cB.hitBox1Scaled.x,
                    tB.position.y + cB.hitBox1Scaled.y,
                    cB.hitBox1Scaled.w,
                    cB.hitBox1Scaled.h
                };
            }

            if (Collision::AABB(hbA, hbB)) {
                // std::cout << cA.tag << "Hit: " << cB.tag << std::endl;
                CollisionKey key = makeKey(entityA, entityB);
                currentCollisions.insert(key);
                if (!activeCollisions.contains(key)) {
                    world.getEventManager().emit(CollisionEvent{entityA, entityB, CollisionState::Enter});
                }
                world.getEventManager().emit(CollisionEvent{entityA, entityB, CollisionState::Stay});
            }
        }
    }

    for (auto &key: activeCollisions) {
        if (!currentCollisions.contains(key)) {
            world.getEventManager().emit(CollisionEvent{key.first, key.second, CollisionState::Exit});
        }
    }
    activeCollisions = std::move(currentCollisions); //update with current collisions
}

std::vector<Entity *> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity> > &entities) {
    std::vector<Entity *> collidables;
    for (auto &e: entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            collidables.push_back(e.get());
        }
    }
    return collidables;
}
