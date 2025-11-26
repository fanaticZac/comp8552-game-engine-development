//
// Created by user on 2025-09-09.
//

#include "../Map.h"
#include <cmath>
#include <iostream>

#include "TextureManager.h"
#include <sstream>
#include <tinyxml2.h>
#include <SDL3/SDL.h>
#include "Game.h"

void Map::load(const char *path, SDL_Texture *ts) {
    tileset = ts;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path);

    auto* mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");
    //parse terrain data
    auto* layer = mapNode->FirstChildElement("layer");
    auto* data = layer->FirstChildElement("data");
    std::string csv = data->GetText();
    std::stringstream ss(csv);
    tileData = std::vector(height, std::vector<int>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::string val;
            //read characters from a ss into val until it hits a comma, or is at the end of the stream
            if (!std::getline(ss, val, ',')) break;
            tileData[i][j] = std::stoi(val);
        }
    }

    // parse collider data
    // auto* objectGroup = layer->NextSiblingElement("objectgroup");
    // // create a for loop with initialization, condition and an increment
    // for (auto* obj = objectGroup->FirstChildElement("object");
    //     obj != nullptr;
    //     obj = obj->NextSiblingElement("object")
    // ){
    //     Collider c;
    //     c.rect.x = obj->FloatAttribute("x");
    //     c.rect.y = obj->FloatAttribute("y");
    //     c.rect.w = obj->FloatAttribute("width");
    //     c.rect.h = obj->FloatAttribute("height");
    //     colliders.push_back(c);
    // }

    // Loop through all object groups
    for (auto* objectGroup = layer->NextSiblingElement("objectgroup");
         objectGroup != nullptr;
         objectGroup = objectGroup->NextSiblingElement("objectgroup"))
    {
        std::string name = objectGroup->Attribute("name");

        if (name == "Collision Layer") {
            for (auto* obj = objectGroup->FirstChildElement("object");
                 obj != nullptr;
                 obj = obj->NextSiblingElement("object"))
            {
                Collider c;
                c.hitBox1base.x = obj->FloatAttribute("x");
                c.hitBox1base.y = obj->FloatAttribute("y");
                c.hitBox1base.w = obj->FloatAttribute("width");
                c.hitBox1base.h = obj->FloatAttribute("height");
                colliders.push_back(c);
            }
        }
        else if (name == "Spawn Layer") {
            for (auto* obj = objectGroup->FirstChildElement("object");
                 obj != nullptr;
                 obj = obj->NextSiblingElement("object"))
            {
                float x = obj->FloatAttribute("x");
                float y = obj->FloatAttribute("y");
                spawnPoints.push_back(Vector2D(x, y));
            }
        }
    }

    // IMAGE LAYER FOR PARALLAX BACKGROUND MOVEMENT
    for (auto* imageLayer = mapNode->FirstChildElement("imagelayer");
       imageLayer != nullptr;
       imageLayer = imageLayer->NextSiblingElement("imagelayer"))
    {
        auto* imageNode = imageLayer->FirstChildElement("image");
        if (!imageNode) continue;

        ImageLayer layer;
        std::string src = imageNode->Attribute("source");
        layer.offsetX = imageLayer->FloatAttribute("offsetx", 0.0f);
        layer.offsetY = imageLayer->FloatAttribute("offsety", 0.0f);
        layer.repeatX = imageLayer->IntAttribute("repeatx", 0) != 0;
        layer.width   = imageNode->IntAttribute("width", 0);
        layer.height  = imageNode->IntAttribute("height", 0);

        // Use your TextureManager to load
        layer.texture = TextureManager::load(src.c_str());
        if (!layer.texture) {
            std::cerr << "Failed to load image layer: " << src << std::endl;
            continue;
        }

        // Assign parallax factor
        std::string name = imageLayer->Attribute("name");
        if (name.find("4") != std::string::npos) layer.parallaxFactor = 1.0f;
        else if (name.find("3") != std::string::npos) layer.parallaxFactor = 0.2f;
        else if (name.find("2") != std::string::npos) layer.parallaxFactor = 0.4f;
        else layer.parallaxFactor = 0.6f;

        imageLayers.push_back(layer);
    }
}

void Map::updateParallaxBackground(float deltaTime) {
    for (auto& layer : imageLayers) {
        layer.offsetX -= layer.parallaxFactor * parallaxBaseSpeed * deltaTime;

        // Correct wrapping for leftward movement
        if (layer.repeatX && layer.offsetX <= -layer.width) {
            layer.offsetX += layer.width;
        }
    }
}

void Map::updateLevelScroll(float deltaTime) {
    // move offset continuously
    scrollOffset += scrollSpeedPx * deltaTime;

    // wrap every full tile (32px)
    if (scrollOffset >= 32.0f)
        scrollOffset -= 32.0f;
}

void Map::drawParallaxBackground(const Camera& cam) {
    for (auto& layer : imageLayers) {
        SDL_FRect dest{};
        dest.w = static_cast<float>(layer.width);
        dest.h = static_cast<float>(layer.height);

        // Combine camera offset with autoscroll
        dest.x = layer.offsetX - cam.view.x * (1.0f - layer.parallaxFactor);
        dest.y = layer.offsetY - cam.view.y * (1.0f - layer.parallaxFactor);

        TextureManager::draw(layer.texture, SDL_FRect{0,0,(float)layer.width,(float)layer.height}, dest);

        // Draw extra copy for repeat
        if (layer.repeatX) {
            SDL_FRect dest2 = dest;
            if (dest.x < 0) dest2.x = dest.x + layer.width;
            else dest2.x = dest.x - layer.width;
            TextureManager::draw(layer.texture, SDL_FRect{0,0,(float)layer.width,(float)layer.height}, dest2);
        }
    }
}

// int mapData[10][15] = {
//     {1, 2, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
// };

// Map::Map() :
// water(TextureManager::load("../assets/water.png")),
// dirt(TextureManager::load("../assets/dirt.png")),
// grass(TextureManager::load("../assets/grass.png")) {
//     load(mapData);
//
//     // what part of the image do we want
//     // This would be changed if using a sprite sheet
//     // -- where we want to a subsection from the source image
//     src.x = src.y = 0;
//     src.w = src.h = 32;
//
//     // Where do we want to display it
//     dest.x = dest.y = 0;
//     // How big do we want to display it
//     dest.w = dest.h = 64;
// }

// Map::~Map() {
//     if (water) SDL_DestroyTexture(water);
//     if (dirt) SDL_DestroyTexture(dirt);
//     if (grass) SDL_DestroyTexture(grass);
// }
//
// void Map::load(int data[10][15]) {
//     for (int row = 0; row < 10; row++) {
//         for (int col = 0; col < 15; col++) {
//             map[row][col] = data[row][col];
//         }
//     }
// }

void Map::draw(const Camera& cam) {
    drawParallaxBackground(cam);
    drawTiles(cam);
}

void Map::drawTiles(const Camera &cam) {
    SDL_FRect src{}, dest{};
    dest.w = dest.h = 32;

    // int type = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = tileData[row][col];

            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(row) * dest.h;

            // dest.x = std::round(worldX - cam.view.x);
            dest.x = std::round(worldX - cam.view.x - scrollOffset); // for scroll offset
            dest.y = std::round(worldY - cam.view.y);

            // AUTOMATICALLY DO TILE PROCESSING USING MAP - NO MORE SWITCH.
            if (type > 0) { // 0 means "empty tile" in Tiled
                int tilesetColumns = 9;       // from .tsx
                int tileSize = 32;            // from .tsx

                // Convert Tiled's 1-based GID to 0-based index
                int tileIndex = type - 1;

                int tileX = tileIndex % tilesetColumns;
                int tileY = tileIndex / tilesetColumns;

                src.x = tileX * tileSize;
                src.y = tileY * tileSize;
                src.w = tileSize;
                src.h = tileSize;

                TextureManager::draw(tileset, src, dest);
            }

            // Draw one extra tile at the right edge to fill the gap for looping
            int firstTile = tileData[row][0];
            SDL_FRect extraSrc = src;
            SDL_FRect extraDest = dest;
            extraDest.x = std::round(width * tileSize - cam.view.x - scrollOffset);
            int tilesetColumns = 9;  // same as above
            int tileIndex = firstTile - 1;
            int tileX = tileIndex % tilesetColumns;
            int tileY = tileIndex / tilesetColumns;
            extraSrc.x = tileX * tileSize;
            extraSrc.y = tileY * tileSize;
            TextureManager::draw(tileset, extraSrc, extraDest);

        }
    }
}