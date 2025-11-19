//
// Created by user on 2025-11-17.
//

#ifndef COMP8552_FINAL_PROJECT_HITBOXDEBUGSYSTEM_H
#define COMP8552_FINAL_PROJECT_HITBOXDEBUGSYSTEM_H
#include <memory>
#include <vector>
#include <SDL3/SDL_render.h>
#include "Component.h"
#include "Entity.h"


class HitBoxDebugSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>> &entities);

private:

};

#endif //COMP8552_FINAL_PROJECT_HITBOXDEBUGSYSTEM_H