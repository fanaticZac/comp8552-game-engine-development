//
// Created by user on 2025-11-17.
//

#ifndef COMP8552_FINAL_PROJECT_LEVELGENERATIONSYSTEM_H
#define COMP8552_FINAL_PROJECT_LEVELGENERATIONSYSTEM_H
#include <memory>
#include <vector>

#include "AssetManager.h"
#include "Component.h"
#include "Entity.h"
#include "MovementSystem.h"
#include "TextureManager.h"

class World; // forward declaration

class LevelGenerationSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>> &entities, World &world, float dt);

    void spawnWallObstacle(World &world);

    void spawnCarObstacle(World &world);

    static void spawnEnemy1(World &world);

private:
    // TODO - This need work and variability
    float spawnTimer = 15.0f;
    float spawnInterval = 30.0f;
    float wallSpawnTimer = 1.0f;
    float wallSpawnInterval = 30.0f;
    int maxWalls = 10;
    float carSpawnTimer = 1.0f;
    float carSpawnInterval = 30.0f;
    int maxCars = 10;
};

#endif //COMP8552_FINAL_PROJECT_LEVELGENERATIONSYSTEM_H