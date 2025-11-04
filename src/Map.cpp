//
// Created by user on 2025-09-09.
//

#include "../Map.h"
#include <cmath>
#include "TextureManager.h"
#include <sstream>
#include <tinyxml2.h>

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
                c.rect.x = obj->FloatAttribute("x");
                c.rect.y = obj->FloatAttribute("y");
                c.rect.w = obj->FloatAttribute("width");
                c.rect.h = obj->FloatAttribute("height");
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

void Map::draw(const Camera &cam) {
    SDL_FRect src{}, dest{};
    dest.w = dest.h = 32;

    // int type = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = tileData[row][col];

            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(col) * dest.h;

            dest.x = std::round(worldX - cam.view.x);
            dest.y = std::round(worldY - cam.view.y);

            // dest.x = static_cast<float>(col) * dest.w; // the size we set;
            // dest.y = static_cast<float>(row) * dest.h;

            switch (type) {

                case 1:
                    //dirt
                    src.x = 0;
                    src.y = 0;
                    src.w = 32;
                    src.h = 32;
                    break;
                case 2:
                    //grass
                    src.x = 32;
                    src.y = 0;
                    src.w = 32;
                    src.h = 32;
                    break;
                case 4:
                    //water
                    src.x = 32;
                    src.y = 32;
                    src.w = 32;
                    src.h = 32;
                    break;
                default:
                    break;
            }

            TextureManager::draw(tileset, src, dest);

        }
    }
}