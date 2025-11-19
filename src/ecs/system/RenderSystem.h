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

        // NOTE: Sort - by groundY to enable objects to appear in order of depth (lasers/player/enemies) --------
        // latest rendered is on top
        std::vector<Entity *> sorted;
        sorted.reserve(entities.size());

        for (auto &e: entities)
            sorted.push_back(e.get());
        std::stable_sort(sorted.begin(), sorted.end(), compareByGroundY);

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

                // Shadow
                // TODO - REMOVE HARDCODED VALUES here - I'm using 64x64 shadow pics scaled to 192....
                // TODO - and then am adjusting here for laser reflections also - is messy.
                if (entity->hasComponent<Shadow>()) {
                    auto &shadow = entity->getComponent<Shadow>();
                    SDL_FRect dst;
                    dst.x = (entity->hasComponent<Projectile>())? shadow.renderX - 96 - cam.view.x: shadow.renderX - 32 - cam.view.x;
                    dst.y = shadow.renderY - 32 - cam.view.y;
                    dst.w = (entity->hasComponent<Projectile>())? 192 * shadow.currentScale : 64 * shadow.currentScale;
                    dst.h = 64 * shadow.currentScale;

                    SDL_SetTextureAlphaMod(shadow.texture, shadow.currentAlpha);
                    TextureManager::draw(shadow.texture, SDL_FRect{0, 0, 64, 64}, dst);
                }

                TextureManager::draw(sprite.texture, sprite.src, sprite.dst);

                // Draw debug sprite on top - so comes after
                if (entity->hasComponent<DebugSprite>()) {
                    auto& debugSprite = entity->getComponent<DebugSprite>();
                    // Keep it same as normal render, offset by camera
                    SDL_FRect dst = debugSprite.dst;
                    dst.x -= cam.view.x;
                    dst.y -= cam.view.y;
                    TextureManager::draw(debugSprite.texture, debugSprite.src, dst);
                }
            }
        }
    }

private:
    static bool compareByGroundY(Entity *a, Entity *b) {
        bool aRenderable = a->hasComponent<Transform>() && a->hasComponent<Sprite>();
        bool bRenderable = b->hasComponent<Transform>() && b->hasComponent<Sprite>();

        if (!aRenderable && !bRenderable)
            return false;

        if (!aRenderable && bRenderable)
            return true;

        if (aRenderable && !bRenderable)
            return false;

        return a->getComponent<Transform>().groundY <
               b->getComponent<Transform>().groundY;
    }
};

#endif //COMP8552_FINAL_PROJECT_RENDERSYSTEM_H
