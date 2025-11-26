//
// Created by user on 2025-09-16.
//

#ifndef COMP8552_FINAL_PROJECT_WORLD_H
#define COMP8552_FINAL_PROJECT_WORLD_H

#include <memory>
#include <vector>

#include "AnimationSystem.h"
#include "CameraSystem.h"
#include "ScaleObjectByDistanceSystem.h"
#include "CollisionSystem.h"
#include "DestructionSystem.h"
#include "EnemyBehaviorSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "KeyboardInputSystem.h"
#include "MainMenuSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "EventManager.h"
#include "HitBoxDebugSystem.h"
#include "LevelGenerationSystem.h"
#include "PhysicsSystem.h"
#include "../Map.h"
#include "SpawnTimerSystem.h"
#include "SceneType.h"
#include "ShadowSystem.h"
#include "ShootingSystem.h"

//  Entity Manager
class World {
    Map map;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    KeyboardInputSystem keyboardInputSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    EventManager eventManager;
    SpawnTimerSystem spawnTimerSystem;
    DestructionSystem destructionSystem;
    EventResponseSystem eventResponseSystem{*this};
    MainMenuSystem mainMenuSystem;
    PhysicsSystem physicsSystem;
    ShadowSystem shadowSystem;
    ScaleObjectByDistanceSystem scaleObjectByDistanceSystem;
    ShootingSystem shootingSystem;
    EnemyBehaviorSystem enemyBehaviorSystem;
    LevelGenerationSystem levelGenerationSystem;
    HitBoxDebugSystem hitBoxDebugSystem;
    public:
    World() = default;
    void update(float dt, const SDL_Event& event, SceneType sceneType) {

        if (sceneType == SceneType::MainMenu) {
            mainMenuSystem.update(event);
        } else {
            keyboardInputSystem.update(entities, event, dt);
            //keyboord inputs -> movement, so order matters

            physicsSystem.update(entities, dt);
            movementSystem.update(entities, dt);
            collisionSystem.update(*this);
            animationSystem.update(entities, dt);
            cameraSystem.update(entities);
            shadowSystem.update(entities);
            scaleObjectByDistanceSystem.update(entities);
            spawnTimerSystem.update(entities, dt);
            enemyBehaviorSystem.update(entities, dt);
            shootingSystem.update(entities, *this, dt);
            levelGenerationSystem.update(entities, *this, dt);
            hitBoxDebugSystem.update(entities); // Uncomment for visual hitboxs
            destructionSystem.update(entities);
            map.updateParallaxBackground(dt);
            map.updateLevelScroll(dt);
        }
        // FINALLY
        syncronizeEntities();
        cleanup();
    }
    void render() {
        // map.draw();
        for (auto& entity: entities) {
            if (entity->hasComponent<Camera>()) {
                map.draw(entity->getComponent<Camera>());
            }
        }

        renderSystem.render(entities);
    }

    Entity& createEntity() {
        // we use emplace instead of push so we don't create copy
        entities.emplace_back(std::make_unique<Entity>());
        return *entities.back();
    }

    Entity& createDeferredEntities() {
        deferredEntities.emplace_back(std::make_unique<Entity>());
        return *deferredEntities.back();
    }

    std::vector<std::unique_ptr<Entity>>& getEntities() {
        return entities;
    }
    void cleanup() {
        //use a lambda predicate to remove all inactive entities
        std::erase_if(entities, [](std::unique_ptr<Entity>& e) {
            return !e->isActive();
        });
    }

    void syncronizeEntities() {
        if (!deferredEntities.empty()) {
            //push back all deferred entitites to the entitities vector
            //using move so we don't copy
            std::move(
                deferredEntities.begin(),
                deferredEntities.end(),
                std::back_inserter((entities)));
            // clearing the creation buffer
            deferredEntities.clear();
        }
    }
    EventManager& getEventManager() {return eventManager;}

    Map& getMap() {return map;}
};

#endif //COMP8552_FINAL_PROJECT_WORLD_H