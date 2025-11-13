//
// Created by user on 2025-11-08.
//

#include "PhysicsSystem.h"

#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "Map.h"


void PhysicsSystem::update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
    for (auto& entity : entities) {
        if (entity->hasComponent<Transform>() &&
            entity->hasComponent<Physics>() &&
            entity->hasComponent<Boost>() &&
            entity->hasComponent<HitBox>() &&
            entity->hasComponent<Input>() &&
            entity->hasComponent<Velocity>()) {
            auto& transform = entity->getComponent<Transform>();
            auto& physics = entity->getComponent<Physics>();
            auto& boost = entity->getComponent<Boost>();
            auto& velocity = entity->getComponent<Velocity>();
            auto& hitBox = entity->getComponent<HitBox>();
            auto& input = entity->getComponent<Input>();

            // Take the unadulterated input from Velocity componenet
            velocity.direction.normalize();

            // Add base movement speed from movementSystem.
            physics.velocity += velocity.direction * velocity.speed * dt;

            // Gravity
            physics.velocity.y += physics.gravityPixelsPerSecond2 * dt; // positive = downward

            // Boost (upward movement)
            if (boost.boosting) {

                physics.velocity.y = -boost.boostYVelocity; // negative = upward impulse
            }
            // In air: ignore inputs, just keep existing velocity.x
            // no change to physics.velocity.x
            // Grounded: only move if input given
            if (physics.grounded && velocity.direction.x == 0.0f) {
                physics.velocity.x = 0.0f;
            }

            // Apply Max speed cap
            if (physics.velocity.x > physics.maxSpeed) physics.velocity.x = physics.maxSpeed;
            if (physics.velocity.x < -physics.maxSpeed) physics.velocity.x = -physics.maxSpeed;

            // Integrate movement
            transform.position.x += physics.velocity.x * dt;
            // transform.position.y += physics.velocity.y * dt;

            // This accounts for the 45 degree depth screen tilt in 'fake' 2.5D depth movement
            // This is experimental
            if (input.moveBottom && !input.moveTop && boost.boosting) {
                // Appears to float, only groundY is changing / appa
            }else if (!input.moveBottom && input.moveTop && boost.boosting) {
                // transform.position.y += physics.velocity.y * dt * 2;
            }else {
                transform.position.y += physics.velocity.y * dt;
            }

            // Apply drag (air friction)
            // Apply friction
            // float friction = physics.grounded ? physics.groundFriction : physics.airDrag;
            // physics.velocity.x *= (1.0f - friction * dt);

            // TODO - MOVE TO COLLUSION SYSTEM - probably - pondering
            // --- Ground collision ---
            if (transform.position.y > transform.groundY) { // y increases downward
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
            float characterVerticalBoundarySpacing = 12.0f; // Don't change lightly - debug exact outputs - been used for parameters in CharacterScale
            float characterHorizontalBoundarySpacing = 48.0f;

            // Top
            if (transform.position.y + hitBox.hitBox2offsetY - characterVerticalBoundarySpacing <= Map::screenTop) {
                transform.position.y = Map::screenTop - hitBox.hitBox2offsetY + characterVerticalBoundarySpacing;
            }

            // Left
            if (transform.position.x + hitBox.hitBox2offsetY + hitBox.hitBox2width - characterHorizontalBoundarySpacing <= Map::screenLeft) {
                transform.position.x = Map::screenLeft - hitBox.hitBox2offsetY - hitBox.hitBox2width + characterHorizontalBoundarySpacing;
            }

            // Right
            if (transform.position.x + transform.width - hitBox.hitBox2offsetY - hitBox.hitBox2width + characterHorizontalBoundarySpacing >= Map::screenRight) {
                transform.position.x = Map::screenRight - transform.width + hitBox.hitBox2offsetY + hitBox.hitBox2width - characterHorizontalBoundarySpacing;
            }
        }
    }
}


