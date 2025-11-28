//
// Created by user on 2025-09-16.
//

#ifndef COMP8552_FINAL_PROJECT_RENDERSYSTEM_H
#define COMP8552_FINAL_PROJECT_RENDERSYSTEM_H

#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "TextureManager.h"

class RenderSystem {
public:
    void render(const std::vector<std::unique_ptr<Entity> > &entities) {
        Entity *cameraEntity = nullptr;
        //Find camera
        for (auto &e: entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }
        if (!cameraEntity) return; // no camera
        auto &cam = cameraEntity->getComponent<Camera>();

        //Find player
        Entity *playerEntity = nullptr;
        for (auto &e: entities) {
            if (e->hasComponent<Player>()) {
                playerEntity = e.get();
                break;
            }
        }

        // Get obstacle below player for shadow system
        Entity *obstacleUnderPlayer = nullptr;
        if (playerEntity) {
            auto &playerTransform = playerEntity->getComponent<Transform>();
            auto &playerCollider = playerEntity->getComponent<Collider>();

            for (auto &e: entities) {
                if (!e->hasComponent<Obstacle>()) continue;

                auto &oT = e->getComponent<Transform>();
                auto &oC = e->getComponent<Collider>();

                if (playerTransform.groundZ > oT.groundZ) {
                    break;
                }
                if (playerTransform.groundZ < oT.groundZ - oT.height + oT.depth) {
                    break;
                }

                float HB_scaledAndPositionedNewX =
                        playerTransform.position.x - playerCollider.hitBox1Scaled.x +
                        (playerTransform.width - playerCollider.hitBox1Scaled.w) / 2.0f;

                float playerHB_Left = HB_scaledAndPositionedNewX + playerCollider.hitBox1Scaled.x;

                float playerHB_Right = playerHB_Left + playerCollider.hitBox1Scaled.w;

                float wallHB_Left = oT.position.x + oC.hitBox3Scaled.x;
                float wallHB_Right = wallHB_Left + oC.hitBox3Scaled.w;

                bool overlapHorizontal = playerHB_Right >= wallHB_Left &&
                                    playerHB_Left <= wallHB_Right;

                // bool overlapDepth = playerTransform.groundZ <= oT.groundZ && playerTransform.groundZ >= oC.hitBox3Scaled.y;

                if (overlapHorizontal) {
                    obstacleUnderPlayer = e.get();
                    break;
                }
            }
        }


        // NOTE: Sort - by groundY to enable objects to appear in order of depth (lasers/player/enemies) --------
        // latest rendered is on top
        std::vector<Entity *> sorted;
        sorted.reserve(entities.size());

        for (auto &e: entities)
            sorted.push_back(e.get());
        std::stable_sort(sorted.begin(), sorted.end(),
                         [](Entity *a, Entity *b) {
                             bool aRenderable = a->hasComponent<Transform>() && a->hasComponent<Sprite>();
                             bool bRenderable = b->hasComponent<Transform>() && b->hasComponent<Sprite>();

                             auto &aTransform = a->getComponent<Transform>();
                             auto &bTransform = b->getComponent<Transform>();

                             if (!aRenderable && !bRenderable) return false;
                             if (!aRenderable && bRenderable) return true;
                             if (aRenderable && !bRenderable) return false;

                             // --- SPECIAL CASE: player behind obstacle ---
                             // if (playerEntity) {
                             //     auto &playerProperties = playerEntity->getComponent<Player>();
                             //     int obstacleID = playerProperties.isStandingBehindObstacleEntityID;
                             //
                             //     if (obstacleID != -1) {
                             //         // If a is the player and b is the obstacle, a comes first
                             //         if (a == playerEntity && b->getID() == obstacleID) return true;
                             //         // If b is the player and a is the obstacle, b comes first
                             //         if (b == playerEntity && a->getID() == obstacleID) return false;
                             //     }
                             // }

                             // --- DEFAULT: sort by groundZ ---
                             // Don't recommend changing - it works for the projectiles and player so player can move behind back and front of cube with offset working.
                             float aGroundZ = (a->hasComponent<Obstacle>())
                                                  ? aTransform.groundZ - (aTransform.height - aTransform.depth)
                                                  : aTransform.groundZ;
                             float bGroundZ = (b->hasComponent<Obstacle>())
                                                  ? bTransform.groundZ - (bTransform.height - bTransform.depth)
                                                  : bTransform.groundZ;

                             return aGroundZ < bGroundZ;
                         }
        );

        // NOTE: After sort - we check if we need to adjust player and wall order for 3D effect
        // if (playerEntity) {
        //     auto& playerProperties = playerEntity->getComponent<Player>();
        //
        //     if (playerProperties.isStandingBehindObstacleEntityID != -1) {
        //         int behindID = playerProperties.isStandingBehindObstacleEntityID;
        //
        //         int playerIndex = -1;
        //         int obstacleIndex = -1;
        //
        //         // Find indices of player and the obstacle in the sorted list
        //         for (int i = 0; i < sorted.size(); i++) {
        //             if (sorted[i] == playerEntity) playerIndex = i;
        //             if (sorted[i]->getID() == behindID) obstacleIndex = i;
        //         }
        //
        //         // Move obstacle in front of player if needed
        //         if (playerIndex != -1 && obstacleIndex != -1 && obstacleIndex < playerIndex) {
        //             std::swap(sorted[playerIndex], sorted[obstacleIndex]);
        //         }
        //     }
        // }


        for (auto &entity: sorted) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                //Return ref for efficiency
                auto &t = entity->getComponent<Transform>();
                auto &sprite = entity->getComponent<Sprite>();
                // sprite.dst.x = t.position.x;
                // sprite.dst.y = t.position.y;

                // NOTE: ScaleThisObjectByDistance overwrites sprite.dst values - can't be overwritten
                // NOTE: I think only because only enemy and player have ScaleThisObjectByDistance
                if (entity->hasComponent<ScaleThisObjectByDistance>()) {
                    sprite.dst.x -= cam.view.x;
                    sprite.dst.y -= cam.view.y;
                } else {
                    sprite.dst.x = t.position.x - cam.view.x;
                    sprite.dst.y = t.position.y - cam.view.y;
                }

                // if the entity has animation, update hte source rect;
                if (entity->hasComponent<Animation>()) {
                    auto &anim = entity->getComponent<Animation>();
                    sprite.src = anim.clips[anim.currentClip].frameIndices[anim.currentFrame];
                }

                // Player/enemy - flash red on damage
                if (entity->hasComponent<Health>()) {
                    auto &health = entity->getComponent<Health>();
                    if (health.damageFlash) {
                        // Shade sprite red
                        SDL_SetTextureColorMod(sprite.texture, 255, 0, 0); // red
                    } else {
                        SDL_SetTextureColorMod(sprite.texture, 255, 255, 255); // normal
                    }
                }

                // Shadow
                // TODO - REMOVE HARDCODED VALUES here - I'm using 64x64 shadow pics scaled to 192....
                // TODO - and then am adjusting here for laser reflections also - is messy.
                if (entity->hasComponent<Shadow>()) {
                    auto &shadow = entity->getComponent<Shadow>();
                    SDL_FRect dst;
                    dst.x = (entity->hasComponent<Projectile>())
                                ? shadow.renderX - 96 - cam.view.x
                                : shadow.renderX - 32 - cam.view.x;
                    if (entity->hasComponent<Player>() && obstacleUnderPlayer)
                        dst.y = shadow.renderY - 32 - obstacleUnderPlayer->getComponent<Transform>().depth - cam.view.y;
                    else
                        dst.y = shadow.renderY - 32 - cam.view.y;
                    dst.w = (entity->hasComponent<Projectile>()) ? 192 * shadow.currentScale : 64 * shadow.currentScale;
                    dst.h = 64 * shadow.currentScale;

                    SDL_SetTextureAlphaMod(shadow.texture, shadow.currentAlpha);
                    TextureManager::draw(shadow.texture, SDL_FRect{0, 0, 64, 64}, dst);
                }

                TextureManager::draw(sprite.texture, sprite.src, sprite.dst);

                // Draw debug sprite on top - so comes after
                if (entity->hasComponent<DebugSprites>()) {
                    auto &dbg = entity->getComponent<DebugSprites>();

                    for (auto &s: dbg.sprites) {
                        SDL_FRect dst = s.dst;
                        dst.x -= cam.view.x;
                        dst.y -= cam.view.y;
                        TextureManager::draw(s.texture, s.src, dst);
                    }
                }
            }
        }
    }

private:
    // static bool compareByGroundY(Entity *a, Entity *b) {
    //     bool aRenderable = a->hasComponent<Transform>() && a->hasComponent<Sprite>();
    //     bool bRenderable = b->hasComponent<Transform>() && b->hasComponent<Sprite>();
    //     auto &aTransform = a->getComponent<Transform>();
    //     auto &bTransform = a->getComponent<Transform>();
    //
    //     if (!aRenderable && !bRenderable)
    //         return false;
    //
    //     if (!aRenderable && bRenderable)
    //         return true;
    //
    //     if (aRenderable && !bRenderable)
    //         return false;
    //
    //     // For walls only
    //     // 192x192 is image of cube, 128x182 is cube within it.
    //     // Player should be on top when player free in front of back edge of image.
    //     float aGroundZ = (a->hasComponent<ObstacleTag>()) ? aTransform.groundZ - aTransform.depth : aTransform.groundZ;
    //     float bGroundZ = (b->hasComponent<ObstacleTag>()) ? bTransform.groundZ - bTransform.depth : bTransform.groundZ;
    //
    //     return aGroundZ < bGroundZ;
    // }
};

#endif //COMP8552_FINAL_PROJECT_RENDERSYSTEM_H
