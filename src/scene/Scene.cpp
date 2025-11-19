//
// Created by user on 2025-10-21.
//

#include <iostream>
#include "Scene.h"
#include "AssetManager.h"
#include "ScaleObjectByDistanceSystem.h"
#include "Game.h"
#include "TextureManager.h"

Scene::Scene(SceneType sceneType, const char *sceneName, const char *mapPath, const int windowWidth, const int windowHeight) : name(sceneName), type(sceneType) {

    if (sceneType == SceneType::MainMenu){
        //camera - needs to be here to render - but just SDL prereq
        auto &cam = world.createEntity();
        cam.addComponent<Camera>();

        //menu
        auto menu(world.createEntity());
        auto menuTransform = menu.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);

        SDL_Texture *text = TextureManager::load("../assets/+old_assets/menu.png");
        SDL_FRect menuSrc{0, 0, (float) windowWidth, (float) windowHeight};
        SDL_FRect menuDst{menuTransform.position.x, menuTransform.position.y, menuSrc.w, menuSrc.h};
        menu.addComponent<Sprite>(text, menuSrc, menuDst);
        return;
    }

    // load our map
    // map = new Map();
    world.getMap().load(mapPath, TextureManager::load("../assets/level/TileSheet1.png"));
    // world.getMap().load(mapPath, TextureManager::load("../assets/+old_assets/tileset.png"));
    // make map colliders entities
    for (auto &collider: world.getMap().colliders) {
        auto& e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, 1.0f);
        auto& c = e.addComponent<Collider>("wall");
        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;

        // TO ADD THE COLLIDER DEBUG
        // SDL_Texture* tex = TextureManager::load("../assets/+old_assets/tileset.png");
        // SDL_FRect colSrc {0, 32, 32, 32};
        // SDL_FRect colDst {c.rect.x, c.rect.y, c.rect.w, c.rect.h};
        // e.addComponent<Sprite>(tex, colSrc, colDst);
    }

    // Add camera
    auto& cam = world.createEntity();
    SDL_FRect camView{};
    camView.w = windowWidth;
    camView.h = windowHeight;
    cam.addComponent<Camera>(camView, world.getMap().width * 32, world.getMap().height * 32); // hardcoded

    // Add item
    for (auto& spawnPoint: world.getMap().spawnPoints) {
        std::cout << "Spawn Point x: " << spawnPoint.x << " y: " << spawnPoint.y << std::endl;
        auto& item(world.createEntity());
        auto& itemTransform = item.addComponent<Transform>(spawnPoint, 0.0f, 1.0f);
        SDL_Texture* itemTex = TextureManager::load("../assets/+old_assets/coin.png");
        SDL_FRect itemSrc{ 0, 0, 32, 32};
        SDL_FRect itemDst { itemTransform.position.x, itemTransform.position.y, 32, 32};
        // item.addComponent<Sprite>(itemTex, itemSrc, itemDst);

        auto& itemCollider = item.addComponent<Collider>("item");
        itemCollider.rect.w = itemDst.w;
        itemCollider.rect.h = itemDst.h;
    }

    // PLAYER START
    //load player
    // player = new GameObject("../assets/+old_assets/ball.png", 0, 0);

    //add entities
    auto& player(world.createEntity());

    // THE x, y here  JUST ADJUSTS SPAWN POSITION
    auto& playerTransform = player.addComponent<Transform>(Vector2D(0,0), 0.0f, 1.0f, 192, 192, 6*5); // last 2 character size, and depthWidth
    player.addComponent<Velocity>();

    Animation anim = AssetManager::getAnimation("player");
    player.addComponent<Animation>(anim);

    // SDL_Texture* tex = TextureManager::load("../assets/+old_assets/ball.png");
    // SDL_Texture* tex = TextureManager::load("../assets/+old_assets/mario.png");
    SDL_Texture* tex = TextureManager::load("../assets/player/PlayerSheet_192x192.png");

    // SDL_FRect playerSrc {0, 0, 32, 44};a
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];

    // SDL_FRect playerDst {playerTransform.position.x, playerTransform.position.y, 64, 88};
    SDL_FRect playerDst {playerTransform.position.x, playerTransform.position.y, playerTransform.width, playerTransform.height}; // last 2 character size
    player.addComponent<Sprite>(tex, playerSrc, playerDst, playerTransform.width, playerTransform.height);

    auto& playerCollider = player.addComponent<Collider>("player");
    playerCollider.rect.w = playerDst.w;
    playerCollider.rect.h = playerDst.h;

    player.addComponent<PlayerTag>();
    player.addComponent<Health>(Game::gameState.playerHealth);
    player.addComponent<Boost>();
    player.addComponent<Physics>();
    player.addComponent<Input>();
    player.addComponent<ScaleThisObjectByDistance>();
    // I counted hit box pixels at 32x32 but this is scaled to 192x192, so times everything by 6
    // leftOffset: 14, width:5 (14-18), topOffset
    // player.addComponent<HitBox>(13, 11, 5, 21, 13, 9, 5, 23); // 2 hit boxes, one for boosting
    player.addComponent<HitBox>(6*13, 6*11, 6*5, 6*21, 6*13, 6*9, 6*5, 6*23); // ^*at 192x192 - 2 hit boxes, 2nd for boosting
    player.addComponent<Gun>(6*24, 0, 6*24, 0); // Gun tip is embedded to match in image.
    // Shadow for player
    player.addComponent<Shadow>();
    auto& shadow = player.getComponent<Shadow>();
    SDL_Texture* shadowTex = TextureManager::load("../assets/shadow4.png"); // simple ellipse or transparent sprite
    shadow.texture = shadowTex;
    // PLAYER END


    // OLD
    // auto& spawner (world.createEntity());
    // Transform t = spawner.addComponent<Transform>(Vector2D(windowWidth/2, windowHeight - 5), 0.0f, 1.0f);
    // spawner.addComponent<TimedSpawner>(2.0f, [this, t] {
    //    // create those birds
    //    auto& e(world.createDeferredEntities());
    //     e.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
    //     e.addComponent<Velocity>(Vector2D(0, -1), 100.0f);
    //
    //     Animation anim = AssetManager::getAnimation("enemy");
    //     e.addComponent<Animation>(anim);
    //
    //     // SDL_Texture* tex = TextureManager::load("../assets/+old_assets/animations/bird_anim.png");
    //
    //     SDL_FRect src = {0, 0, 192, 192};
    //     SDL_FRect dest {t.position.x, t.position.y, 192, 192};
    //     e.addComponent<Sprite>(tex, src, dest);
    //
    //     Collider c = e.addComponent<Collider>("projectile");
    //     c.rect.w = dest.w;
    //     c.rect.h = dest.h;
    //
    //     e.addComponent<ProjectileTag>();
    // });


    // State

    auto &state (world.createEntity());
    state.addComponent<SceneState>();
}