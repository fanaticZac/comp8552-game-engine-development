//
// Created by user on 2025-11-10.
//

#ifndef COMP8552_FINAL_PROJECT_SHADOWSYSTEM_H
#define COMP8552_FINAL_PROJECT_SHADOWSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class ShadowSystem {
public:
    void update(std::vector<std::unique_ptr<Entity> > &entities) {
        for (auto &entity: entities) {
            if (entity->hasComponent<Transform>() &&
                entity->hasComponent<Shadow>()) {
                auto &t = entity->getComponent<Transform>();
                auto &s = entity->getComponent<Shadow>();

                // NOTE: SHADOW SYSTEM WAS SETUP FOR 64X IMAGES AND HASN'T BEEN ADJUSTED - JUST USE THAT SIZE FOR NOW
                // Note: this is used for player shadow as well as reflective bit on ground for enemy laser blasts (to inform depth)
                // Note: -- this can be adjusted later in render (STRETCH)

                // Place shadow directly under entity
                s.renderX = t.position.x + (t.width / 2);
                s.renderY = t.groundY + t.width;

                // std::cout << "Shadow position is  X: " << s.renderX << ", Y: " << t.groundY << std::endl;

                // Shrink shadow and change opaque alpha based on vertical position
                float height = t.groundY - t.position.y;
                if (height < 0) height = 0;

                float strength = 1000.0f;
                if (entity->hasComponent<Projectile>()) strength = 2000;

                float ratio = std::min(height / strength, 1.0f); // 720 is screen height
                s.currentScale = s.maxScale - ratio * (s.maxScale - s.minScale);
                s.currentAlpha = static_cast<Uint8>(255 * (1.0f - ratio));
                // std::cout << "pos.y=" << t.position.y
                //           << " groundY=" << t.groundY
                //           << " height=" << (t.position.y - t.groundY)
                //           << " ratio=" << std::min((t.position.y - t.groundY) / 500.0f, 1.0f)
                //           << " alpha=" << (int)s.currentAlpha
                //           << std::endl;
            }
        }
    }
};


#endif //COMP8552_FINAL_PROJECT_SHADOWSYSTEM_H
