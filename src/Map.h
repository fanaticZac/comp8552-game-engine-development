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

    std::vector<std::vector<int>> tileData;
    std::vector<Collider> colliders;
    std::vector<Vector2D> spawnPoints;

    // Map - I don't know why width and height are hardcoded, heigh and width are magic values just to avoid changing this right now.
    // - I made the map for this specific size anyway...
    int width{}, height{}; // 1280x720
    static const int screenLeft = 0;
    static const int screenRight = 1280;
    static const int screenTop = 0;
    static const int screenBottom = 720;
    static const int playerGroundHeight = 384; // 12 tiles * 32 pixels
    static const int skyAreaHeight = 352; // 11 tiles * 32 pixels

    static const int playerGroundTop = screenTop + skyAreaHeight;
    static const int playerGroundBottom = screenBottom;
    static const int playerSkyTop = screenTop;


    // Tile Layer
    const float tileSize = 32.0f;
    float scrollOffset = 0.0f;   // pixel offset from 0 to tile size
    float scrollSpeedPx = 180.0f; // pixels/second (96px is 3 tiles (or 1 large map block from tilesheet))
    SDL_Texture *tileset = nullptr;

    // Add image layer for parallax background loading
    float parallaxBaseSpeed = 10.0f; // pixels/second

    struct ImageLayer {
        SDL_Texture* texture = nullptr;
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        bool repeatX = false;
        int width = 0;
        int height = 0;
        float parallaxFactor = 1.0f;
    };
    std::vector<ImageLayer> imageLayers;

    // void load(int data[10][15]);
    // teacher: named draw when one thing, render when multiple things

    void load(const char *path, SDL_Texture *ts);

    void updateParallaxBackground(float deltaTime);

    void updateLevelScroll(float deltaTime);

    void drawParallaxBackground(const Camera& cam);

    void draw(const Camera &cam);

    void drawTiles(const Camera &cam);

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