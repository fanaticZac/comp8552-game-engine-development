//
// Created by user on 2025-11-13.
//

#include "ShootingSystem.h"

#include <iostream>

#include "World.h"

void ShootingSystem::update(const std::vector<std::unique_ptr<Entity> > &entities, World &world, float dt) {
    for (auto &entity: entities) {
        if (((entity->hasComponent<Player>() && entity->hasComponent<Input>()) || (
                 entity->hasComponent<Enemy>() && entity->hasComponent<EnemyBehavior>())) &&
            entity->hasComponent<Transform>() &&
            entity->hasComponent<Velocity>() &&
            entity->hasComponent<Gun>() &&
            entity->hasComponent<Sprite>() &&
            entity->hasComponent<Collider>()) {
            if (cooldownTimer > 0.0f)
                cooldownTimer -= dt;

            auto &transform = entity->getComponent<Transform>();
            auto &gun = entity->getComponent<Gun>();
            auto &sprite = entity->getComponent<Sprite>();

            if (cooldownTimer > 0.0f)
                continue;

            if (entity->hasComponent<Player>()) {
                auto &input = entity->getComponent<Input>();
                if (!input.shootGun2) continue;
            }

            if (entity->hasComponent<Enemy>()) {
                auto &behavior = entity->getComponent<EnemyBehavior>();
                if (!behavior.shooting) continue;
            }

            // NOTE: WE DO NOT APPLY ScaleObjectByDistanceSystem TO THESE LASER BEAMS BECAUSE THEY HAVE TO MATCH UP USING...
            // NOTE: ... THE ORIGINAL ENTITIES PARAMETERS (AKA SCALING, GUN TIP (1 OR 2), ETC)
            // NOTE: THIS WAS ACTUALLY SUPER ANNOYING TO TRY AND ALIGN LASER BEAMS TO GUN WITH SCALING SYSTEM THAT CENTERS CHARACTER HORIZONTALLY...
            // NOTE ... AND SCALED DOWN TO BOTTOM OF IMAGE (to match shadow)- UNTIL i JUST MADE THEM LASER IMAGE MATCH HORIZONTAL GUN TIP (SAME Y OFFSET)
            // NOTE - WHERE THE HEIGHT OF GUN TIP MATCHES SO SAME EFFECT OCCURS WITH SAME CODE. JUST X THEN NEEDS TO BE OFFSET TO X OFFSET GUN TIP - WORKS SMOOTHLY

            float scaledWidth = sprite.dst.w;
            // Already scaled by ScaleObjectByDistanceSystem = sprite.baseWidth * sprite.currentScale;
            float scaledHeight = sprite.dst.h;
            // Already scaled by ScaleObjectByDistanceSystem = sprite.baseHeight * sprite.currentScale;

            // Create deferred laser entity
            Entity &laserEntity = world.createDeferredEntities();


            // Sprite
            if (entity->hasComponent<Player>() && entity->hasComponent<Physics>()) {
                float direction = 1;
                auto &laserProjectile = laserEntity.addComponent<Projectile>();
                laserProjectile.ownerID = entity->getID();
                auto &physics = entity->getComponent<Physics>();
                laserEntity.addComponent<Velocity>(Velocity{{direction, 0}, laserProjectile.speed});

                if (physics.grounded) {
                    float scaledAndPositionedNewX =
                            transform.position.x + gun.gunTip1OffsetXScaled + (sprite.baseWidth - scaledWidth) / 2.0f;
                    float scaledAndPositionedNewY =
                            transform.position.y + gun.gunTip1OffsetYScaled + (sprite.baseHeight - scaledHeight);
                    SDL_Texture *laserTex = TextureManager::load("../assets/weapon/laser1.png");
                    auto &laserTransform = laserEntity.addComponent<Transform>(Vector2D(
                                                                                   scaledAndPositionedNewX,
                                                                                   scaledAndPositionedNewY),
                                                                               0.0f,
                                                                               1.0f, 192, 192, 6 * 1);
                    laserTransform.groundZ = transform.groundZ;
                    // Here we apply the scaling data from existing sprite to match scale but add scaled guntip position to align with scaled sprite
                    laserEntity.addComponent<Sprite>(Sprite{
                        .texture = laserTex,
                        .src = SDL_FRect{0, 0, transform.width, transform.height},
                        .dst = SDL_FRect{
                            scaledAndPositionedNewX,
                            scaledAndPositionedNewY,
                            sprite.dst.w,
                            sprite.dst.h
                        },
                        .currentScale = sprite.currentScale
                    });

                    // Add hitbox for laserbeam (this one is natively 192x192)
                    // This should just be current height for both. to much current gun tip height
                    // Collider
                    // SDL_FRect dest {laserTransform.position.x, laserTransform.position.y, 192, 192};
                    laserEntity.addComponent<Collider>("projectile",
                                                       SDL_FRect(6 * 0 * sprite.currentScale,
                                                                 6 * 15 * sprite.currentScale,
                                                                 6 * 32 * sprite.currentScale,
                                                                 6 * 1 * sprite.currentScale)
                    );
                } else {
                    // BOOSTING
                    float scaledAndPositionedNewX =
                            transform.position.x + gun.gunTip2OffsetXScaled + (sprite.baseWidth - scaledWidth) / 2.0f;
                    float scaledAndPositionedNewY =
                            transform.position.y + gun.gunTip2OffsetYScaled + (sprite.baseHeight - scaledHeight);
                    SDL_Texture *laserTex = TextureManager::load("../assets/weapon/laser1-boost.png");
                    auto &laserTransform = laserEntity.addComponent<Transform>(Vector2D(
                                                                                   scaledAndPositionedNewX,
                                                                                   scaledAndPositionedNewY),
                                                                               0.0f,
                                                                               1.0f, 192, 192, 6 * 1);
                    laserTransform.groundZ = transform.groundZ;
                    // Here we apply the scaling data from existing sprite to match scale but add scaled guntip position to align with scaled sprite
                    laserEntity.addComponent<Sprite>(Sprite{
                        .texture = laserTex,
                        .src = SDL_FRect{0, 0, transform.width, transform.height},
                        .dst = SDL_FRect{
                            scaledAndPositionedNewX,
                            scaledAndPositionedNewY,
                            sprite.dst.w,
                            sprite.dst.h
                        },
                        .currentScale = sprite.currentScale
                    });

                    // Add hitbox for laserbeam (this one is natively 192x192)
                    laserEntity.addComponent<Collider>("projectile",
                                                       SDL_FRect(6 * 0 * sprite.currentScale,
                                                                6 * 13 * sprite.currentScale,
                                                                6 * 32 * sprite.currentScale,
                                                                6 * 1 * sprite.currentScale));
                }
            }

            //ENEMY BEAM
            if (entity->hasComponent<Enemy>()) {
                // minus gun point here for
                float scaledAndPositionedNewX = transform.position.x - gun.gunTip1OffsetXScaled + (
                                                    sprite.baseWidth - scaledWidth) / 2.0f;
                float scaledAndPositionedNewY = transform.position.y - gun.gunTip1OffsetYScaled + (
                                                    sprite.baseHeight - scaledHeight);
                // // Laser tag
                auto &laserProjectile = laserEntity.addComponent<Projectile>(600);
                laserProjectile.ownerID = entity->getID();

                float direction = -1;
                // Velocity (movement)
                laserEntity.addComponent<Velocity>(Velocity{{direction, 0}, laserProjectile.speed});
                // Transform (position)
                auto &laserTransform = laserEntity.addComponent<Transform>(
                    Vector2D(scaledAndPositionedNewX,
                             scaledAndPositionedNewY),
                    0.0f, 1.0f, 192, 192, 6 * 4);
                laserTransform.groundZ = transform.groundZ;

                SDL_Texture *laserTex = TextureManager::load("../assets/weapon/enemyLaser-quick-shot2.png");

                // for
                laserEntity.addComponent<Sprite>(Sprite{
                    .texture = laserTex,
                    .src = SDL_FRect{0, 0, transform.width, transform.height},
                    .dst = SDL_FRect{
                        scaledAndPositionedNewX,
                        scaledAndPositionedNewY,
                        scaledWidth,
                        scaledHeight
                    },
                    .currentScale = sprite.currentScale
                });

                // Add hitbox for laserbeam
                laserEntity.addComponent<Collider>("projectile",
                   SDL_FRect( 6 * 0 * sprite.currentScale,
                    6 * 14 * sprite.currentScale,
                    6 * 32 * sprite.currentScale,
                    6 * 4 * sprite.currentScale)
                );

                // laser shadow - to show depth to player
                laserEntity.addComponent<Shadow>(1.0, 0.8, 1.0, 180);
                auto &enemyLaserShadow = laserEntity.getComponent<Shadow>();
                SDL_Texture *enemyLaserShadowTex = TextureManager::load("../assets/enemyLaser-quick-shot2-shadow.png");
                // simple ellipse or transparent sprite
                enemyLaserShadow.texture = enemyLaserShadowTex;
            }

            // Reset cooldown
            cooldownTimer = 1.0f / gun.gun2FireRatePerSecond;

            // DEBUG: log everything
            std::cout << std::fixed;

            // std::cout << "\n-- Local Variables --\n";
            // std::cout << "  direction:          " << direction << "\n";
            // std::cout << "  shotSpriteImage:    " << shotSpriteImage << "\n";
            // std::cout << "  scaledWidth:        " << scaledWidth << "\n";
            // std::cout << "  scaledHeight:       " << scaledHeight << "\n";
            // // std::cout << "  laserImageWidth:    " << laserImageWidth << "\n";
            // // std::cout << "  laserImageHeight:   " << laserImageHeight << "\n";
            //
            // std::cout << "\n-- Player --\n";
            // std::cout << "  Position:           (" << transform.position.x << ", " << transform.position.y << ")\n";
            // std::cout << "  Boosting:           " << (boost.boosting ? "YES" : "NO") << "\n";
            //
            // std::cout << "\n-- Sprite --\n";
            // std::cout << "  Base size:          (" << sprite.baseWidth << ", " << sprite.baseHeight << ")\n";
            // std::cout << "  Current scale:      " << sprite.currentScale << "\n";
            //
            // std::cout << "\n-- Gun (raw base offsets) --\n";
            // std::cout << "  Tip1 base:          (" << gun.gunTip1BaseOffsetX << ", " << gun.gunTip1BaseOffsetY << ")\n";
            // std::cout << "  Tip2 base:          (" << gun.gunTip2BaseOffsetX << ", " << gun.gunTip2BaseOffsetY << ")\n";
            //
            // std::cout << "\n-- Gun (scaled offsets) --\n";
            // std::cout << "  Tip1 scaled:        (" << gun.gunTip1OffsetX << ", " << gun.gunTip1OffsetY << ")\n";
            // std::cout << "  Tip2 scaled:        (" << gun.gunTip2OffsetX << ", " << gun.gunTip2OffsetY << ")\n";
            //
            // std::cout << "\n-- Gun (used tip) --\n";
            // // std::cout << "  World spawn:        (" << tipX << ", " << tipY << ")\n";
            //
            // std::cout << "\n-- HitBoxes --\n";
            // std::cout << "  HB1 offset:         (" << hitBox.hitBox1offsetX << ", " << hitBox.hitBox1offsetY << ")\n";
            // std::cout << "  HB2 offset:         (" << hitBox.hitBox2offsetX << ", " << hitBox.hitBox2offsetY << ")\n";
            //
            // std::cout << "\n-- Laser --\n";
            // std::cout << "  Speed:              " << laser.speed << "\n";
            //
            // std::cout << "\n-- Cooldown --\n";
            // std::cout << "  Timer reset to:     " << cooldownTimer << "\n";
            //
            // std::cout << "===========================================\n\n";

            break;
        }
    }
}
