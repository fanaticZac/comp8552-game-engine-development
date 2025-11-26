//
// Created by user on 2025-11-17.
//

#include "HitBoxDebugSystem.h"
#include "TextureManager.h"

void HitBoxDebugSystem::update(const std::vector<std::unique_ptr<Entity> > &entities) {
    SDL_Texture *tex = TextureManager::load("../assets/+old_assets/water.png");
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(tex, 128); // 50% transparency
    SDL_Texture *tex2 = TextureManager::load("../assets/+old_assets/grass.png");
    SDL_SetTextureBlendMode(tex2, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(tex2, 128); // 50% transparency
    SDL_Texture *tex3 = TextureManager::load("../assets/+old_assets/dirt.png");
    SDL_SetTextureBlendMode(tex2, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(tex2, 128); // 50% transparency

    for (auto &entityPtr: entities) {
        auto &e = *entityPtr;
        if (!e.hasComponent<Transform>() || !e.hasComponent<Collider>()) continue;

        auto &t = e.getComponent<Transform>();
        auto &c = e.getComponent<Collider>();
        std::vector<Sprite> sprites;

        SDL_FRect hb1Dst{
            t.position.x + c.hitBox1Scaled.x,
            t.position.y + c.hitBox1Scaled.y,
            c.hitBox1Scaled.w,
            c.hitBox1Scaled.h
        };
        SDL_FRect colSrc1{0, 32, 32, 32};
        sprites.push_back({tex, colSrc1, hb1Dst});

        SDL_FRect hb2Dst{
            t.position.x + c.hitBox2Scaled.x,
            t.position.y + c.hitBox2Scaled.y,
            c.hitBox2Scaled.w,
            c.hitBox2Scaled.h
        };
        SDL_FRect colSrc2{0, 32, 32, 32};
        sprites.push_back({tex2, colSrc2, hb2Dst});

        // F
        if (c.hitbox3_isActive) {
            SDL_FRect hb3Dst{
                t.position.x + c.hitBox3Scaled.x,
                t.position.y + c.hitBox3Scaled.y,
                c.hitBox3Scaled.w,
                c.hitBox3Scaled.h
            };
            SDL_FRect colSrc3{0, 32, 32, 32};
            sprites.push_back({tex3, colSrc3, hb3Dst});
        }


        e.addComponent<DebugSprites>(sprites);
    }
}
