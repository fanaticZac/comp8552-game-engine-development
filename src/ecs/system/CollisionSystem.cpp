//
// Created by user on 2025-10-04.
//

#include "CollisionSystem.h"

#include <iostream>

#include "Collision.h"
#include "World.h"

// Has 2 important functions
// - position collider with transform
// - checking for collisions
// Note: this can be improved for effeciency
void CollisionSystem::update(World &world) {

    // Get a list of entities that have transforms and colliders
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());

    //update all collider positions first
    for (auto entity: collidables) {
            auto& t = entity->getComponent<Transform>();
            auto& c = entity->getComponent<Collider>();
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
        // auto& tA = entityA->getComponent<Transform>();
        auto& cA = entityA->getComponent<Collider>();

        // Inner loop
        // Check for collisions
        for (size_t j = i + 1; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto& cB = entityB->getComponent<Collider>();

            if (Collision::AABB(cA, cB)) {
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

    for (auto& key: activeCollisions) {
        if (!currentCollisions.contains(key)) {
            world.getEventManager().emit(CollisionEvent{key.first, key.second, CollisionState::Exit});
        }
    }
    activeCollisions = std::move(currentCollisions); //update with current collisions
}

std::vector<Entity *> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity> > &entities) {
    std::vector<Entity *> collidables;
    for (auto& e: entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            collidables.push_back(e.get());
        }
    }
    return collidables;
}
