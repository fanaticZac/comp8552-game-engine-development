//
// Created by user on 2025-11-08.
//

#ifndef COMP8552_FINAL_PROJECT_PHYSICSSYSTEM_H
#define COMP8552_FINAL_PROJECT_PHYSICSSYSTEM_H
#include <memory>
#include <vector>

#include "Entity.h"

class PhysicsSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt);
private:

};
#endif //COMP8552_FINAL_PROJECT_PHYSICSSYSTEM_H