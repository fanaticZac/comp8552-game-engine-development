//
// Created by user on 2025-11-04.
//

#ifndef COMP8552_FINAL_PROJECT_KEYBOARDINPUTSYSTEM_H
#define COMP8552_FINAL_PROJECT_KEYBOARDINPUTSYSTEM_H

#include <memory>
#include <vector>
#include <SDL3/SDL_events.h>

#include "Component.h"
#include "Entity.h"
#include "Map.h"

class KeyboardInputSystem {
public:
    std::unordered_map<SDL_Keycode, bool> keyDown;

    void update(const std::vector<std::unique_ptr<Entity> > &entities, const SDL_Event &event, float dt) {
        if (event.type == SDL_EVENT_KEY_DOWN)
            keyDown[event.key.key] = true;
        else if (event.type == SDL_EVENT_KEY_UP)
            keyDown[event.key.key] = false;

        for (auto &entity: entities) {
            if (entity->hasComponent<PlayerTag>() &&
                entity->hasComponent<Transform>() &&
                entity->hasComponent<Input>() &&
                entity->hasComponent<Velocity>() &&
                entity->hasComponent<Gun>() &&
                entity->hasComponent<HitBox>()) {
                auto &input = entity->getComponent<Input>();
                auto &gun = entity->getComponent<Gun>();
                auto &boost = entity->getComponent<Boost>();
                auto &physics = entity->getComponent<Physics>();
                auto &transform = entity->getComponent<Transform>();
                auto &velocity = entity->getComponent<Velocity>();
                auto &hitBox = entity->getComponent<HitBox>();

                // For convenience - mostly for animations so we can draw distinction between
                // pressing a button and effect occurring such as velocity that could be
                // interrupted with other systems (like physics)
                input.moveTop = keyDown[SDLK_W];
                input.moveBottom = keyDown[SDLK_S];
                input.moveLeft = keyDown[SDLK_A];
                input.moveRight = keyDown[SDLK_D];
                input.boost = keyDown[SDLK_SPACE]; // Boost Handled
                input.shootGun1 = keyDown[SDLK_LEFT];
                input.shootGun2 = keyDown[SDLK_RIGHT];

                boost.boosting = input.boost;
                gun.gun1Shooting = (!input.shootGun2) ? input.shootGun1 : false;
                gun.gun2Shooting = (!input.shootGun1) ? input.shootGun2 : false;

                // For now, groundY is be changed here rather than movementSystem so we will backup old here
                transform.oldGroundY = transform.groundY;

                // Movement
                float groundYMovementIncrement = 120.0f; // per second with dt - was 2.0 before
                float directionXMovementIncrement = 1; // Is normalized later so will be 1 either way
                velocity.direction = {0, 0};
                velocity.speed = 180.0f; // will be velocity.x speed

                // Movement Boundary
                float characterVerticalBoundarySpacing = 12.0f; // Don't change lightly - debug exact outputs - been used for parameters in CharacterScale
                float characterHorizontalBoundarySpacing = 24.0f;

                // BOUNDARY CODE SPECIFICALLY - BEST NOT TO TOUCH THIS - FULLY TESTED BOUNDARY - IT'S TIME-CONSUMING TO RETEST......

                // Still honing this in. Adding the v.direction.x for W/S allows it to move in alignment with the angled level
                if (physics.grounded || boost.boosting) { // CAN ONLY MOVE WHEN BOOSTING OR RUNNING
                    if (input.moveTop) {
                        // boundary
                        if (transform.groundY + transform.height - characterVerticalBoundarySpacing - (groundYMovementIncrement * dt) >= Map::playerGroundTop) {
                            // movement
                            transform.groundY -= groundYMovementIncrement * dt;

                            // velocity.direction.x += 2.0f; // kept here to remind me how to change to diagonal movement

                            if (physics.grounded) {
                                transform.position.y = transform.groundY;
                            }
                        }
                    }

                    if (input.moveBottom) {
                        if (transform.groundY + transform.height  + (groundYMovementIncrement * dt) <= Map::playerGroundBottom) {
                            transform.groundY += groundYMovementIncrement * dt;

                            // velocity.direction.x += 2.0f; // kept here to remind me where to change to diagonal movement

                            if (physics.grounded) {
                                transform.position.y = transform.groundY;
                            }
                        }
                    }

                    // Hitbox additions only matter here because groundy stuff is through the bottom of image,
                    // and the character always touches bottom. Left right we have to factor hit box

                    if (input.moveLeft) {
                        //boundary
                        if (transform.position.x - directionXMovementIncrement + hitBox.hitBox2offsetYScaled + hitBox.hitBox2widthScaled - characterHorizontalBoundarySpacing >= Map::screenLeft) {
                            velocity.direction.x -= directionXMovementIncrement;
                        }
                    }

                    if (input.moveRight) {
                        if (transform.position.x + transform.width + directionXMovementIncrement - hitBox.hitBox2offsetYScaled - hitBox.hitBox2widthScaled + characterHorizontalBoundarySpacing <= Map::screenRight) {
                        velocity.direction.x += directionXMovementIncrement;
                        }
                    }
                }
            }
        }
    }
};


#endif //COMP8552_FINAL_PROJECT_KEYBOARDINPUTSYSTEM_H
