//
// Created by user on 2025-11-17.
//

#include "LevelGenerationSystem.h"

#include <iostream>

#include "World.h"

void LevelGenerationSystem::update(const std::vector<std::unique_ptr<Entity> > &entities, World &world, float dt) {
    bool enemyExists = false;

    int activeWalls = 0;
    int activeCars = 0;

    // We only want to generate enemy if no enemy exists
    for (auto &e: entities) {
        if (e->hasComponent<Obstacle>() && e->isActive()) activeWalls++;
        if (e->hasComponent<CarObstacle>() && e->isActive()) activeCars++;

        if (e->hasComponent<Enemy>() && e->isActive()) {
            enemyExists = true;
            // break;
        }
    }

    // Run spawners
    for (auto &entity: entities) {
        if (!enemyExists) {
            spawnTimer -= dt;
            if (spawnTimer <= 0.0f) {
                spawnEnemy1(world);
                spawnTimer = spawnInterval;
            }
        } else {
            spawnTimer = spawnInterval;
        }

        // 1) WALL spawner: Uses normal timed spawner
        if (activeWalls < maxWalls) {
            wallSpawnTimer -= dt;
            if (wallSpawnTimer <= 0.0f) {
                spawnWallObstacle(world);
                wallSpawnTimer = wallSpawnInterval;
            }
        }

        // 2) Car spawner
        if (activeCars < maxCars) {
            carSpawnTimer -= dt;
            if (carSpawnTimer <= 0.0f) {
                spawnCarObstacle(world);
                carSpawnTimer = carSpawnInterval;
            }
        }
    }
}


void LevelGenerationSystem::spawnWallObstacle(World &world) {
    // std::cout << "Wall obstacle created" << std::endl;

    SDL_Texture *tex = TextureManager::load("../assets/level/Obstacles1.png");

    const int frameW = 192;
    const int frameH = 192;
    const int frameD = 128; // same as vertical offset 64 - depth is fake but used for hitbox horizontal alignment in 2.5D

    // Spawn position
    // int minY = world.getMap().playerGroundTop - 128;
    int maxY = world.getMap().screenBottom - 192;

    // Just manually doing this
    int startingYArray[] = {224, 288, 352, 416, 480, 544};
    float baseY = (float) startingYArray[rand() % 6];
    float baseX = world.getMap().screenRight; // Spawn off-screen right
    int verticalStep = frameH - frameD; // difference is offset of the cube.
    int segmentPotentials = (maxY - baseY) / verticalStep;
    if (segmentPotentials < 1) segmentPotentials = 1;
    int segments = rand() % segmentPotentials + 1;

    for (int i = 0; i < segments; i++) {
        // Offset each segment 64px right & 64px down - for alignment of rows
        float x = baseX + (i * 64.0f);
        float y = baseY + (i * 64.0f);


        auto &wall(world.createEntity());
        wall.addComponent<Obstacle>();
        // wall.addComponent<ScaleThisObjectByDistance>();

        // Transform
        auto &transform = wall.addComponent<Transform>(
            Vector2D(x, y),
            0.0f,
            1.0f,
            frameW,
            frameH,
            frameD
        );
        transform.groundZ = y;

        // Moving left
        wall.addComponent<Velocity>(Vector2D(-1, 0), 180.0f);

        // Sprite (first 192x192 region)
        SDL_FRect src{0, 0, (float) frameW, (float) frameH};
        SDL_FRect dst{x, y, (float) frameW, (float) frameH};

        auto &sprite = wall.addComponent<Sprite>();
        sprite.texture = tex;
        sprite.src = src;
        sprite.dst = dst;
        sprite.baseWidth = frameW;
        sprite.baseHeight = frameH;

        // Collider
        // NOTE: For player, HB1 is on ground, HB2 is when boosting
        // NOTE: For cube obstacle, HB1 is front of box, HB2 is back of box, and
        // NOTE: - depth is used to create that 45 degree angle matching the sprite
        auto &hb = wall.addComponent<Collider>("wall", SDL_FRect(0, 0, 4 * 32, 4 * 32), SDL_FRect(64, 64, 4 * 32, 4 * 32));

        // std::cout << "Created wall segment " << i+1 << "/" << segments << std::endl;
    }
}
//
//
// void LevelGenerationSystem::spawnCarObstacle(World &world) {
//     // std::cout << "Wall obstacle created" << std::endl;
//
//     SDL_Texture *tex = TextureManager::load("../assets/level/Obstacles2.png");
//
//     const int frameW = 16 * 9;
//     const int frameH = 16 * 9;
//     const int frameD = 11*9;
//
//     // Spawn position
//     // int minY = world.getMap().playerGroundTop - 128;
//     int maxY = world.getMap().screenBottom - 16;
//
//     // Just manually doing this
//     int startingYArray[] = {272, 326, 380, 434, 488, 542};
//     float baseY = (float) startingYArray[rand() % 6];
//     float baseX = world.getMap().screenRight; // Spawn off-screen right
//     int offset = frameH - frameD; // difference is offset of the cube.
//     int segmentPotentials = (maxY - baseY) / offset;
//     if (segmentPotentials < 1) segmentPotentials = 1;
//     int segments = 1; //rand() % segmentPotentials + 1;
//
//     for (int i = 0; i < segments; i++) {
//         // Offset each segment 64px right & 64px down - for alignment of rows
//         float x = baseX + (i * offset);
//         float y = baseY + (i * offset);
//
//
//         auto &wall(world.createEntity());
//         wall.addComponent<Obstacle>();
//         wall.addComponent<CarObstacle>();
//         // wall.addComponent<ScaleThisObjectByDistance>();
//
//         // Transform
//         auto &transform = wall.addComponent<Transform>(
//             Vector2D(x, y),
//             0.0f,
//             1.0f,
//             frameW,
//             frameH,
//             frameD
//         );
//         transform.groundZ = y - offset;
//
//         // Moving left
//         wall.addComponent<Velocity>(Vector2D(-1, 0), 180.0f);
//
//         // Sprite (first 192x192 region)
//         SDL_FRect src{0, 0, (float) frameW, (float) frameH};
//         SDL_FRect dst{x, y, (float) frameW, (float) frameH};
//
//         auto &sprite = wall.addComponent<Sprite>();
//         sprite.texture = tex;
//         sprite.src = src;
//         sprite.dst = dst;
//         sprite.baseWidth = frameW;
//         sprite.baseHeight = frameH;
//
//         // Collider
//         // NOTE: For player, HB1 is on ground, HB2 is when boosting
//         // NOTE: For cube obstacle, HB1 is front of box, HB2 is back of box, and
//         // NOTE: - depth is used to create that 45 degree angle matching the sprite
//         auto &hb = wall.addComponent<Collider>("wall", SDL_FRect(0, 0, frameD, frameD), SDL_FRect(offset, offset, frameD, frameD));
//
//         // std::cout << "Created wall segment " << i+1 << "/" << segments << std::endl;
//     }
// }



void LevelGenerationSystem::spawnCarObstacle(World &world) {
    // std::cout << "Wall obstacle created" << std::endl;

    SDL_Texture *tex = TextureManager::load("../assets/level/car1.png");

    const int frameW = 32 * 5;
    const int frameH = 25 * 5;
    const int frameD = 16 * 5;

    // Spawn position
    // int minY = world.getMap().playerGroundTop - 128;
    int maxY = world.getMap().screenBottom - 16;

    // Just manually doing this
    int startingYArray[] = {272, 326, 380, 434, 488, 542};
    float baseY = (float) startingYArray[rand() % 6];
    float baseX = world.getMap().screenRight; // Spawn off-screen right
    int offsetW = frameW - frameD; // difference is offset of the cube.
    int offsetH = frameH - frameD; // difference is offset of the cube.
    int segmentPotentials = (maxY - baseY) / offsetH;
    if (segmentPotentials < 1) segmentPotentials = 1;
    int segments = rand() % segmentPotentials + 1;

    for (int i = 0; i < segments; i++) {
        // Offset each segment 64px right & 64px down - for alignment of rows
        float x = baseX + (i * offsetW);
        float y = baseY + (i * offsetH);


        auto &wall(world.createEntity());
        wall.addComponent<Obstacle>();
        wall.addComponent<CarObstacle>();
        // wall.addComponent<ScaleThisObjectByDistance>();

        // Transform
        auto &transform = wall.addComponent<Transform>(
            Vector2D(x, y),
            0.0f,
            1.0f,
            frameW,
            frameH,
            frameD
        );
        transform.groundZ = y - offsetH-8*5;

        // Moving left
        wall.addComponent<Velocity>(Vector2D(-1, 0), 180.0f);

        // Sprite (first 192x192 region)
        SDL_FRect src{0, 0, (float) frameW, (float) frameH};
        SDL_FRect dst{x, y, (float) frameW, (float) frameH};

        auto &sprite = wall.addComponent<Sprite>();
        sprite.texture = tex;
        sprite.src = src;
        sprite.dst = dst;
        sprite.baseWidth = frameW;
        sprite.baseHeight = frameH;

        // Collider
        // NOTE: For player, HB1 is on ground, HB2 is when boosting
        // NOTE: For cube obstacle, HB1 is front of box, HB2 is back of box, and
        // NOTE: - depth is used to create that 45 degree angle matching the sprite
        auto &hb = wall.addComponent<Collider>("wall", SDL_FRect(0, 8*5, frameD, frameD-8*5), SDL_FRect(offsetW, offsetH+8*5, frameD, frameD - 8*5));

        // std::cout << "Created wall segment " << i+1 << "/" << segments << std::endl;
    }
}

void LevelGenerationSystem::spawnEnemy1(World &world) {
    std::cout << "Enemy created" << std::endl;

    auto &enemy1(world.createEntity());
    SDL_Texture *enemy1_tex = TextureManager::load("../assets/enemy/EnemySheet1_192x192.png");
    auto &enemy1Transform = enemy1.addComponent<Transform>(
        Vector2D(world.getMap().screenRight - 192 - 10, world.getMap().screenBottom / 2), 0.0f, 1.0f, 192, 192,
        6 * 11);
    enemy1.addComponent<Velocity>();
    // Animation
    Animation enemy1Animations = AssetManager::getAnimation("enemy1");
    enemy1.addComponent<Animation>(enemy1Animations);
    SDL_FRect enemy1Src = enemy1Animations.clips[enemy1Animations.currentClip].frameIndices[0];
    SDL_FRect enemy1Dst{
        enemy1Transform.position.x, enemy1Transform.position.y, enemy1Transform.width, enemy1Transform.height
    }; // last 2 character size
    enemy1.addComponent<Sprite>(enemy1_tex, enemy1Src, enemy1Dst, enemy1Transform.width, enemy1Transform.height);
    // Collider
    // auto &enemy1Collider = enemy1.addComponent<Collider>("enemy1");
    // enemy1Collider.hitBox1base.w = enemy1Dst.w;
    // enemy1Collider.hitBox1base.h = enemy1Dst.h;
    std::cout << "Enemy created" << std::endl;
    // Other Systems
    enemy1.addComponent<Enemy>();
    enemy1.addComponent<ScaleThisObjectByDistance>();
    enemy1.addComponent<EnemyBehavior>();
    enemy1.addComponent<Health>(2);
    enemy1.addComponent<Collider>("enemy1", SDL_FRect(6 * 11, 6 * 0, 6 * 13, 6 * 32), SDL_FRect(6 * 11, 6 * 0, 6 * 13, 6 * 32));
    // ^*at 192x192 - 2 hit boxes, 2nd for boosting
    enemy1.addComponent<Gun>(6 * 16, 6 * 0, 6 * 16, 6 * 0);

    // // Shadow for enemy1
    auto &enemyshadow = enemy1.addComponent<Shadow>();
    SDL_Texture *enemyShadowTex = TextureManager::load("../assets/shadow3.png"); // simple ellipse or transparent sprite
    enemyshadow.texture = enemyShadowTex;
}
