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
        Entity* cameraEntity = nullptr;

        //Find camera
        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }

        if (!cameraEntity) return; // no camera
        auto& cam = cameraEntity->getComponent<Camera>();


        for (auto& entity: entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                //Return ref for efficiency
                auto& t = entity->getComponent<Transform>();
                auto& sprite = entity->getComponent<Sprite>();
                // sprite.dst.x = t.position.x;
                // sprite.dst.y = t.position.y;

                if (sprite.currentScale != 1.0f) { // meaning characterSystem modified it
                    // The we want not to overwrite the sprite.dst.x ands sprite.dst.y
                    // As it has been changed manually in characterSystem to accurately center and align with scaled image
                    // Just add camera effect.
                    sprite.dst.x -= cam.view.x;
                    sprite.dst.y  -= cam.view.y;
                } else {
                    sprite.dst.x = t.position.x - cam.view.x;
                    sprite.dst.y = t.position.y - cam.view.y;
                }

                // if the entity has animation, update hte source rect;
                if (entity->hasComponent<Animation>()) {
                    auto& anim = entity->getComponent<Animation>();
                    sprite.src = anim.clips[anim.currentClip].frameIndices[anim.currentFrame];
                }

                // Shadow
                if (entity->hasComponent<Shadow>()) {
                    auto& shadow = entity->getComponent<Shadow>();
                    SDL_FRect dst;
                    dst.x = shadow.renderX - 32 - cam.view.x; // apply camera
                    dst.y = shadow.renderY - 32 - cam.view.y; // apply camera
                    dst.w = 64 * shadow.currentScale;
                    dst.h = 64 * shadow.currentScale;

                    SDL_SetTextureAlphaMod(shadow.texture, shadow.currentAlpha);
                    TextureManager::draw(shadow.texture, SDL_FRect{0, 0, 64, 64}, dst);
                }

                TextureManager::draw(sprite.texture, sprite.src, sprite.dst);
            }
        }
    }
};

#endif //COMP8552_FINAL_PROJECT_RENDERSYSTEM_H