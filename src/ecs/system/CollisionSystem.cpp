//
// Created by user on 2025-10-04.
//

#include "CollisionSystem.h"

#include <cmath>
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
    for (auto entity: collidables) {
        auto &t = entity->getComponent<Transform>();
        auto &c = entity->getComponent<Collider>();
        //sect collider to match position
        c.rect.x = t.position.x;
        c.rect.y = t.position.y;
    }

    std::set<CollisionKey> currentCollisions;

    // Handle collisions
    // Outer loop
    for (size_t i = 0; i < collidables.size(); i++) {
        //update the collider position
        auto entityA = collidables[i];
        auto &tA = entityA->getComponent<Transform>();
        auto &cA = entityA->getComponent<Collider>();
        auto &hA = entityA->getComponent<HitBox>();

        SDL_FRect hbA;
        if (entityA->hasComponent<PlayerTag>() && !entityA->getComponent<Physics>().grounded) {
            hbA = {
                tA.position.x + hA.hitBox2offsetXScaled,
                tA.position.y + hA.hitBox2offsetYScaled,
                hA.hitBox2widthScaled,
                hA.hitBox2heightScaled
            };
        } else {
            hbA = {
                tA.position.x + hA.hitBox1offsetXScaled,
                tA.position.y + hA.hitBox1offsetYScaled,
                hA.hitBox1widthScaled,
                hA.hitBox1heightScaled
            };
        }

        // Inner loop
        // Check for collisions
        for (size_t j = i + 1; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto &cB = entityB->getComponent<Collider>();
            auto &tB = entityB->getComponent<Transform>();
            auto &hB = entityB->getComponent<HitBox>();

            // NOTE: Here we make adjustment to use entities hitbox and ensure they are both within the groundY depth range
            // NOTE: -- to qualify a collision for this system
            // This checks the groundY value against the depthWidth threshold aka how wide Z-index is in the 2.5 dimension

            const float depthDist = std::fabs(tA.groundY - tB.groundY);
            const float depthThreshold = (tA.depthWidth + tB.depthWidth) * 0.5f;
            if (depthDist > depthThreshold) {
                continue;
            }

            SDL_FRect hbB;
            if (entityB->hasComponent<PlayerTag>() && !entityB->getComponent<Physics>().grounded) {
                hbB = {
                    tB.position.x + hB.hitBox2offsetXScaled,
                    tB.position.y + hB.hitBox2offsetYScaled,
                    hB.hitBox2widthScaled,
                    hB.hitBox2heightScaled
                };
            } else {
                hbB = {
                    tB.position.x + hB.hitBox1offsetXScaled,
                    tB.position.y + hB.hitBox1offsetYScaled,
                    hB.hitBox1widthScaled,
                    hB.hitBox1heightScaled
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
