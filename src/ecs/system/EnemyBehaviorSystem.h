//
// Created by user on 2025-11-14.
//

#ifndef COMP8552_FINAL_PROJECT_ENEMYBEHAVIORSYSTEM_H
#define COMP8552_FINAL_PROJECT_ENEMYBEHAVIORSYSTEM_H
#include <chrono>
#include <ctime>
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "Map.h"

class EnemyBehaviorSystem {
public:
    EnemyBehaviorSystem() {
        std::srand((unsigned)std::time(nullptr));
    }

    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {

        for (auto& e : entities) {
            if (!e->hasComponent<EnemyBehavior>() ||
                !e->hasComponent<Transform>() ||
                !e->hasComponent<Velocity>() ||
                !e->hasComponent<Collider>())
            {
                continue;
            }

            auto& behavior  = e->getComponent<EnemyBehavior>();
            auto& transform = e->getComponent<Transform>();
            auto& velocity  = e->getComponent<Velocity>();
            auto& collider    = e->getComponent<Collider>();

            // NOTE: This starting code works properly with the in game boundaries
            // NOTE: -- and allow me to work on the shooting, hit boxes and health components
            // TODO: Make a player following mode for shooting directly at them. (and maybe toggle back and forth from random)
            // TODO: Make a second shooting mode (thinking persistent blast player needs to avoid rather than quick shot)

            behavior.moveTimer += dt;

            if (behavior.moveTimer >= behavior.nextMoveSwitchTime) {
                behavior.moveTimer = 0.0f;

                // Random next switch interval (1–3 seconds)
                behavior.nextMoveSwitchTime = 1.0f + ((rand() % 2000) / 1000.0f);

                // Random X direction: -1 or +1
                behavior.directionX = (rand() % 2 == 0 ? -1.0f : 1.0f);

                // Random Y direction: -1 or +1
                behavior.directionY = (rand() % 2 == 0 ? -1.0f : 1.0f);

                // Random ground bob direction: -1 or +1
                behavior.directionGround = (rand() % 2 == 0 ? -1.0f : 1.0f);
            }

            velocity.direction = Vector2D{0, 0};

            float characterVerticalBoundarySpacing = 12.0f; // Don't change lightly - debug exact outputs - been used for parameters in CharacterScale
            float characterHorizontalBoundarySpacing = 24.0f;
            float minX = Map::screenRight /2;
            float maxX = Map::screenRight - transform.width - characterHorizontalBoundarySpacing;
            float minGround = Map::playerGroundTop - transform.height + characterVerticalBoundarySpacing;
            float maxGround = Map::playerGroundBottom - transform.height + characterVerticalBoundarySpacing;
            float minY = transform.groundZ - transform.height + characterVerticalBoundarySpacing;
            float maxY = transform.groundZ;


            // Horizontal movement
            if (behavior.directionX < 0 && transform.position.x <= minX) {
                behavior.directionX = 1; // flip right
            } else if (behavior.directionX > 0 && transform.position.x >= maxX) {
                behavior.directionX = -1; // flip left
            }
            float moveX = behavior.directionX * behavior.movementSpeedX * dt;
            velocity.direction.x = behavior.directionX;
            velocity.speed = behavior.movementSpeedX;

            // groundy or depth simulation
            if (behavior.directionGround < 0 && transform.groundZ <= minGround) {
                behavior.directionGround = 1; // flip down
            } else if (behavior.directionGround > 0 && transform.groundZ >= maxGround) {
                behavior.directionGround = -1; // flip up
            }
            float moveGround = behavior.directionGround * behavior.movementSpeedGroundY * dt;
            transform.groundZ += moveGround;

            // vertical movement - from current groundY upward
            if (behavior.directionY < 0 && transform.position.y <= minY) {
                behavior.directionY = 1; // flip downward
            } else if (behavior.directionY > 0 && transform.position.y >= maxY) {
                behavior.directionY = -1; // flip upward
            }
            float moveY = behavior.directionY * behavior.movementSpeedY * dt;
            velocity.direction.y = behavior.directionY;
            velocity.speed = behavior.movementSpeedY;

            // Random Shooting logic

            behavior.fireTimer += dt;

            switch (behavior.fireState) {
                case EnemyBehavior::State::Idle:
                    behavior.dying    = false;
                    behavior.charging = false;
                    behavior.shooting = false;

                    if (behavior.fireTimer >= behavior.nextFireTime) {
                        // Begin charging
                        behavior.fireState = EnemyBehavior::State::Charging;
                        behavior.stateTimer = 0.0f;
                    }
                    break;
                case EnemyBehavior::State::Charging:
                    behavior.charging = true;
                    behavior.shooting = false;
                    behavior.dying    = false;

                    behavior.stateTimer += dt;

                    if (behavior.stateTimer >= 1.0f) {
                        behavior.fireState = EnemyBehavior::State::Shooting;
                        behavior.stateTimer = 0.0f;
                    }
                    break;

                case EnemyBehavior::State::Shooting:
                    behavior.charging = false;
                    behavior.shooting = true;
                    behavior.dying    = false;

                    behavior.stateTimer += dt;

                    if (behavior.stateTimer >= 1.0f) {
                        // Fire cycle complete — reset
                        behavior.fireState   = EnemyBehavior::State::Idle;
                        behavior.fireTimer   = 0.0f;
                        behavior.nextFireTime = 1.0f + ((rand() % 8000) / 1000.0f); // 1-8 seconds
                        behavior.stateTimer  = 0.0f;
                    }
                    break;

                case EnemyBehavior::State::Dying:
                {
                    behavior.dying    = true;
                    behavior.shooting = false;
                    behavior.charging = false;

                    // Up and over - to match animation -
                    velocity.speed = 180;
                    velocity.direction = {-1, -1};

                    // Track dying animation time
                    behavior.stateTimer += dt;

                    // NOTE: MATCH animation total
                    // After 1st animation frame - remove
                    if (behavior.stateTimer >= 3.0f/6) {
                        e->deactivateComponent<Shadow>();
                        // e->deactivateComponent<Collider>();
                    }

                    // NOTE: MATCH animation total
                    if (behavior.stateTimer >= 3.0f) {
                        e->destroy();
                    }
                    break;
                }
            }
        }
    }

private:

};

#endif //COMP8552_FINAL_PROJECT_ENEMYBEHAVIORSYSTEM_H