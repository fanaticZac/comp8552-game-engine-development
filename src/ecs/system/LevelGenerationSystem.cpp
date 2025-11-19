//
// Created by user on 2025-11-17.
//

#include "LevelGenerationSystem.h"

#include <iostream>

#include "World.h"

void LevelGenerationSystem::update(const std::vector<std::unique_ptr<Entity> > &entities, World &world, float dt) {
    bool enemyExists = false;

    // We only want to generate enemy if no enemy exists
    for (auto &e: entities) {
        if (e->hasComponent<Enemy>() && e->isActive()) {
            enemyExists = true;
            break;
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

        // TODO - NEXT ADD THE MOVING 'WALL' OBSTACLES. - also check collision code
        // // 1) WALL spawner: Uses normal timed spawner
        // if (entity->hasComponent<TimedSpawner>()) {
        //     auto& spawner = entity->getComponent<TimedSpawner>();
        //     spawner.timer -= dt;
        //     if (spawner.timer <= 0) {
        //         spawner.timer = spawner.spawnInterval;
        //         spawner.spawnCallback();
        //     }
        // }
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
    auto &enemy1Collider = enemy1.addComponent<Collider>("enemy1");
    enemy1Collider.rect.w = enemy1Dst.w;
    enemy1Collider.rect.h = enemy1Dst.h;
    std::cout << "Enemy created" << std::endl;
    // Other Systems
    enemy1.addComponent<Enemy>();
    enemy1.addComponent<ScaleThisObjectByDistance>();
    enemy1.addComponent<EnemyBehavior>();
    enemy1.addComponent<Health>(2);
    enemy1.addComponent<HitBox>(6 * 11, 6 * 0, 6 * 13, 6 * 32, 6 * 11, 6 * 0, 6 * 13, 6 * 32);
    // ^*at 192x192 - 2 hit boxes, 2nd for boosting
    enemy1.addComponent<Gun>(6 * 16, 6 * 0, 6 * 16, 6 * 0);

    // // Shadow for enemy1
    auto &enemyshadow = enemy1.addComponent<Shadow>();
    SDL_Texture *enemyShadowTex = TextureManager::load("../assets/shadow3.png"); // simple ellipse or transparent sprite
    enemyshadow.texture = enemyShadowTex;
}
