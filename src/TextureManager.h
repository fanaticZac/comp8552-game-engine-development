//
// Created by user on 2025-09-09.
//

#ifndef COMP8552_FINAL_PROJECT_TEXTUREMANAGER_H
#define COMP8552_FINAL_PROJECT_TEXTUREMANAGER_H

#include <unordered_map>
#include <string>
#include "SDL3_image/SDL_image.h"

// We can use this to load, cache, and clean textures, total management
class TextureManager {

    static std::unordered_map<std::string, SDL_Texture*> textures;
public:
    static SDL_Texture* load(const char* path);
    static void draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst);
    static void clean();
};



#endif //COMP8552_FINAL_PROJECT_TEXTUREMANAGER_H