//
// Created by user on 2025-10-14.
//

#ifndef COMP8552_FINAL_PROJECT_ANIMATIONCLIP_H
#define COMP8552_FINAL_PROJECT_ANIMATIONCLIP_H

#include <vector>
#include <SDL3/SDL_rect.h>

struct AnimationClip {
    std::vector<SDL_FRect> frameIndices;
};

#endif //COMP8552_FINAL_PROJECT_ANIMATIONCLIP_H