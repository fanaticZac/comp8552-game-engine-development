//
// Created by user on 2025-10-04.
//

#ifndef COMP8552_FINAL_PROJECT_COLLISIONSYSTEM_H
#define COMP8552_FINAL_PROJECT_COLLISIONSYSTEM_H

#include <algorithm>
#include <memory>
#include <set>
#include <vector>
#include "Entity.h"

using CollisionKey = std::pair<Entity*, Entity*>;

// Forward Declaration
class World;

class CollisionSystem {
public:
    void update(World& world);
    std::set<CollisionKey> activeCollisions;
private:
    std::vector<Entity*> queryCollidables(const std::vector<std::unique_ptr<Entity>> &entities);
    CollisionKey makeKey(Entity* entityA, Entity* entityB) {
        return std::minmax(entityA, entityB); //automatically orders pair - smaller number (memory order) first.
    }
};

#endif //COMP8552_FINAL_PROJECT_COLLISIONSYSTEM_H