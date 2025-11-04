//
// Created by user on 2025-09-09.
//

#ifndef COMP8552_FINAL_PROJECT_MAP_H
#define COMP8552_FINAL_PROJECT_MAP_H

// #include "Game.h"
#include <vector>
#include <SDL3/SDL.h>
#include "Component.h"

class Map {
public:
    Map() = default;
    ~Map() = default;

    // void load(int data[10][15]);
    // teacher: named draw when one thing, render when multiple things

    void load(const char *path, SDL_Texture *ts);
    void draw(const Camera &cam);

    SDL_Texture *tileset = nullptr;
    int width{}, height{};
    std::vector<std::vector<int>> tileData;
    std::vector<Collider> colliders;
    std::vector<Vector2D> spawnPoints;

    // private:
    //     //brace initialization
    //     SDL_FRect src{}, dest{};
    //     SDL_Texture* water = nullptr;
    //     SDL_Texture* dirt = nullptr;
    //     SDL_Texture* grass = nullptr;
    //
    //     int map[10][15]{}; // same as = {}; default init
};


#endif //COMP8552_FINAL_PROJECT_MAP_H