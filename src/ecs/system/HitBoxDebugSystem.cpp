//
// Created by user on 2025-11-17.
//

#include "HitBoxDebugSystem.h"
#include "TextureManager.h"

void HitBoxDebugSystem::update(const std::vector<std::unique_ptr<Entity>>& entities) {
    SDL_Texture* tex = TextureManager::load("../assets/+old_assets/water.png");
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(tex, 128); // 50% transparency

    for (auto& entityPtr : entities) {
        auto& e = *entityPtr;
        if (!e.hasComponent<Transform>() || !e.hasComponent<HitBox>()) continue;

        auto& t = e.getComponent<Transform>();
        auto& hb = e.getComponent<HitBox>();

        // Add first hitbox debug sprite if it doesn't exist
        if (!e.hasComponent<DebugSprite>()) {
            SDL_FRect hb1Dst{
                t.position.x + hb.hitBox1offsetXScaled,
                t.position.y + hb.hitBox1offsetYScaled,
                hb.hitBox1widthScaled,
                hb.hitBox1heightScaled
            };
            SDL_FRect colSrc1 {0, 32, 32, 32};
            e.addComponent<DebugSprite>(tex, colSrc1, hb1Dst);

            SDL_FRect hb2Dst{
                t.position.x + hb.hitBox2offsetXScaled,
                t.position.y + hb.hitBox2offsetYScaled,
                hb.hitBox2widthScaled,
                hb.hitBox2heightScaled
            };
            SDL_FRect colSrc2 {0, 32, 32, 32};
            e.addComponent<DebugSprite>(tex, colSrc2, hb2Dst);
        } else {
            auto& debugSprite = e.getComponent<DebugSprite>();
            // Scale the debug sprite with the entity
            debugSprite.dst.x = t.position.x + hb.hitBox1offsetXScaled;
            debugSprite.dst.y = t.position.y + hb.hitBox1offsetYScaled;
            debugSprite.dst.w = hb.hitBox1widthScaled;
            debugSprite.dst.h = hb.hitBox1heightScaled;
        }
    }
}
