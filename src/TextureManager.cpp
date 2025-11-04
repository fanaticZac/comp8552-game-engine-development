//
// Created by user on 2025-09-09.
//

#include "../TextureManager.h"
#include "Game.h"

#include <iostream>
#include <ostream>

// doesn't create extra memory - just grabs reference of object
extern Game *game;

std::unordered_map<std::string, SDL_Texture*> TextureManager::textures;

// load our texture with a path
SDL_Texture* TextureManager::load(const char* path) {
    // What is a surface?
    // a surface represents an image in ram (cpu memory)
    // do we want to do anything with the image before we store it in gpu

    // NEW ON EDIT
    // Caching = storing the result of some work you don't have to repeat the work
    // In this case, check if the texture already exists, or load into memory
    auto it = textures.find(path);
    if (it != textures.end()) { // if the iterator is not at the end, we found it
        return it->second;
    }


    SDL_Surface* tempSurface = IMG_Load(path);
    if (!tempSurface) {
        std::cout << "Failed to load image: " << path << std::endl;
    }
    // Texture is a image on teh gpu (vram)
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, tempSurface);

    // Cleanup Surface
    SDL_DestroySurface(tempSurface);

    if (!texture) {
        std::cout << "Failed to create texture: " << path << std::endl;
        return nullptr;
    }

    //store the new texture in the cache
    textures[path] = texture;

    return texture;
}

void TextureManager::draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst) {
    // Draw the texture  to the screen from Vram
    SDL_RenderTexture (game->renderer, texture, &src, &dst);

}

void TextureManager::clean() {
    for (auto& texture: textures) {
        SDL_DestroyTexture(texture.second);
        texture.second = nullptr;
    }
    textures.clear();
}
