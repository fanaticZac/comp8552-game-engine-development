//
// Created by user on 2025-09-16.
//

#ifndef COMP8552_FINAL_PROJECT_MOVEMENTSYSTEM_H
#define COMP8552_FINAL_PROJECT_MOVEMENTSYSTEM_H

#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"


class MovementSystem {
public:
    void update(std::vector<std::unique_ptr<Entity> > &entities, float dt) {
        for (auto &entity: entities) {
            if (entity->hasComponent<Velocity>() &&
                entity->hasComponent<Transform>()) {
                auto &velocity = entity->getComponent<Velocity>();
                auto &transform = entity->getComponent<Transform>();

                if (!entity->hasComponent<PlayerTag>() && !entity->hasComponent<Physics>()) {
                    Vector2D directionVec = velocity.direction;
                    // Normalizing hasn't happened yet
                    // Vector2D needs an operator function to multiply a float to itself
                    // Example using both operator function orders, check Vector2D function
                    // Vector2D velocityVec1 = directionVec * v.speed;
                    // Vector2D velocityVec2 = v.speed * directionVec;

                    //Normalize so that our vectors are just direction and not magnitude
                    directionVec.normalize();

                    Vector2D velocityVec = directionVec * velocity.speed;
                    transform.position += (velocityVec * dt);


                    Vector2D dir = velocity.direction;
                    dir.normalize();
                    Vector2D velVec = dir * velocity.speed;

                    transform.oldPosition = transform.position;
                    transform.position += velVec * dt;

                    // Determine effective width/height + offsets
                    float offsetX = 0.0f, offsetY = 0.0f;
                    float width = transform.width;
                    float height = transform.height;

                    if (entity->hasComponent<HitBox>()) {
                        auto &hb = entity->getComponent<HitBox>();
                        offsetX = hb.hitBox2offsetXScaled;
                        offsetY = hb.hitBox2offsetYScaled;
                        width = hb.hitBox2widthScaled;
                        height = hb.hitBox2heightScaled;
                    }

                    // // Clamp to screen with spacing
                    // transform.position.x = std::clamp(
                    //     transform.position.x,
                    //     Map::screenLeft + characterHorizontalBoundarySpacing - offsetX,
                    //     Map::screenRight - width - offsetX - characterHorizontalBoundarySpacing
                    // );
                    //
                    // transform.position.y = std::clamp(
                    //     transform.position.y,
                    //     Map::screenTop + characterVerticalBoundarySpacing - offsetY,
                    //     Map::screenBottom - height - offsetY - characterVerticalBoundarySpacing
                    // );
                }
            }
        }
    }

private:
    // Don't change VERTICAL lightly - I used debug outputs based on this in ObjectScaler hardcoding
    // NOTE: DON'T DO THAT ^^^ "Ok ok... sheesh, just this time."
    // TODO: Rectify this as a public const or something. At least pull from same source rather than copying magic numbers
    float characterVerticalBoundarySpacing = 12.0f;
    float characterHorizontalBoundarySpacing = 24.0f;
};

#endif //COMP8552_FINAL_PROJECT_MOVEMENTSYSTEM_H
