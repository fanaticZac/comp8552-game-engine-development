//
// Created by user on 2025-11-08.
//

#include "PhysicsSystem.h"

#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "Map.h"


void PhysicsSystem::update(std::vector<std::unique_ptr<Entity> > &entities, float dt) {
    for (auto &entity: entities) {
        if (entity->hasComponent<Transform>() &&
            entity->hasComponent<Physics>() &&
            entity->hasComponent<Boost>() &&
            entity->hasComponent<HitBox>() &&
            entity->hasComponent<Input>() &&
            entity->hasComponent<Velocity>()) {
            auto &transform = entity->getComponent<Transform>();
            auto &physics = entity->getComponent<Physics>();
            auto &boost = entity->getComponent<Boost>();
            auto &velocity = entity->getComponent<Velocity>();
            auto &hitBox = entity->getComponent<HitBox>();
            auto &input = entity->getComponent<Input>();

            // X axis  - Horizontal movement and physics ----------------------------
            // Take the unadulterated input from Velocity componenet
            velocity.direction.normalize();

            // Add base movement speed from movementSystem.
            // FYI - I had a duplicate dt here by mistake - but that made player move in a realistic acceleration -
            // ^^^ note... I don't think ill use it was it had a mario feel
            physics.velocity += velocity.direction * velocity.speed;


            // In air: ignore inputs, just keep existing velocity.x
            // no change to physics.velocity.x
            // Grounded: only move if input given
            if (physics.grounded && velocity.direction.x == 0.0f) {
                physics.velocity.x = 0.0f;
            }

            // Apply Max speed cap
            if (physics.velocity.x > physics.xMaxSpeed) physics.velocity.x = physics.xMaxSpeed;
            if (physics.velocity.x < -physics.xMaxSpeed) physics.velocity.x = -physics.xMaxSpeed;

            // Integrate movement
            transform.position.x += physics.velocity.x * dt;

            // Y axis - vertical physics - boost and gravity ----------------------------

            // Gravity
            physics.velocity.y += physics.gravityPixelsPerSecond2 * dt; // positive = downward

            // Boost (upward movement)
            if (boost.boosting) {
                // This is experimental 2 - for 45 degree level perspective.
                float depthMin = 127.24f; // bottom of screen
                float depthMax = 526.24f; // top-ish (or whatever max Y for your depth system)
                //
                float depthRatioFasterFarther = 1.0f - std::clamp((transform.groundY - depthMin) / (depthMax - depthMin),
                                                                  0.0f, 1.0f);

                float depthRatioFasterCloser = std::clamp((transform.groundY - depthMin) / (depthMax - depthMin), 0.0f,
                                                          1.0f);

                float depthSpeedFactorFasterFarther = 0.5f + 0.5f * depthRatioFasterFarther;
                float depthSpeedFactorFasterCloser = 0.5f + 0.5f * depthRatioFasterCloser;

                // NOTE: Because th camera perspect appears downwardly towards the leveL at a 45 degree ground angle
                // NOTE: I are trying to create an illusion with the boost using these scales to simulate accomindation for that ( somewhat peacemeal)
                // NOTE: for example, boosting and move up in Y on level otherwise appears that boost is not accelerating upward, but floating at same height
                physics.velocity.y = -boost.boostYVelocity * depthSpeedFactorFasterCloser;

                if (input.moveBottom && !input.moveTop && transform.groundY <= depthMax) {
                    // GOOD, Appears to float, but because player is moving down a 45 degree slope - looks good
                    // only groundY is changing / appa
                } else if (input.moveTop && !input.moveBottom && transform.groundY >= depthMin) {
                    // We add velocity - because it's boosting up y and also moving up 45 degrees in camera persective of level
                    transform.position.y += physics.velocity.y * dt * 1.707; // number is arbitary
                    // transform.position.y += physics.velocity.y * dt * 2;
                } else {
                    // Straight up - normal velocity - map angle doesn't matter
                    transform.position.y += physics.velocity.y * dt;
                }
            } else {
                transform.position.y += physics.velocity.y * dt;
            }

            // Probably won't use
            // Apply drag (air friction)
            // Apply friction
            // float friction = physics.grounded ? physics.groundFriction : physics.airDrag;
            // physics.velocity.x *= (1.0f - friction * dt);

            // TODO - MOVE TO COLLUSION SYSTEM - probably - pondering
            // --- Ground collision ---
            if (transform.position.y > transform.groundY) {
                // y increases downward
                transform.position.y = transform.groundY;
                physics.velocity.y = 0;
                physics.grounded = true;
            } else {
                physics.grounded = false;
            }

            // Other boundary when boosting: left/right/top
            // * This works and against all edges the the black outline of the character stays into the screen
            // * which is good check for hitbox.
            // I will add some padding through now - remove padding to test hit box.

            // BEST NOT TO TOUCH THIS - FULLY TESTED BOUNDARY - IT'S TIME-CONSUMING TO RETEST......
            float characterVerticalBoundarySpacing = 12.0f;  // Don't change VERTICAL lightly - debug exact outputs - been used for parameters in CharacterScale
            float characterHorizontalBoundarySpacing = 24.0f;

            // Top
            if (transform.position.y + hitBox.hitBox2offsetYScaled - characterVerticalBoundarySpacing <= Map::screenTop) {
                transform.position.y = Map::screenTop - hitBox.hitBox2offsetYScaled + characterVerticalBoundarySpacing;
            }

            // Left
            if (transform.position.x + hitBox.hitBox2offsetXScaled - characterHorizontalBoundarySpacing <= Map::screenLeft) {
                transform.position.x = Map::screenLeft - hitBox.hitBox2offsetXScaled + characterHorizontalBoundarySpacing;
            }

            // Right
            if (transform.position.x + hitBox.hitBox2offsetXScaled + hitBox.hitBox2widthScaled + characterHorizontalBoundarySpacing >= Map::screenRight) {
                transform.position.x = Map::screenRight - hitBox.hitBox2offsetXScaled - hitBox.hitBox2widthScaled - characterHorizontalBoundarySpacing;
            }

            // DEBUG - keep
            // printf("vel.x = %f, vel.y = %f, dir.x = %f, dir.y = %f, speed = %f\n",
            //        physics.velocity.x, physics.velocity.y, velocity.direction.x, velocity.direction.y, velocity.speed);
        }
    }
}
