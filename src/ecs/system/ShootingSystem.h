//
// Created by user on 2025-11-13.
//

#ifndef COMP8552_FINAL_PROJECT_SHOOTINGSYSTEM_H
#define COMP8552_FINAL_PROJECT_SHOOTINGSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class World; // forward declaration
class ShootingSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity> > &entities, World& world, float dt);



private:
    float cooldownTimer = 0.0f;
};

#endif //COMP8552_FINAL_PROJECT_SHOOTINGSYSTEM_H
