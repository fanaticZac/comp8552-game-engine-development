//
// Created by user on 2025-11-11.
//

#ifndef COMP8552_FINAL_PROJECT_CHARACTERSCALESYSTEM_H
#define COMP8552_FINAL_PROJECT_CHARACTERSCALESYSTEM_H

#include <vector>
#include <memory>
#include <algorithm>
#include "Entity.h"
#include "Component.h"

class ScaleObjectByDistanceSystem {
    // NOTE: MY GOD BE WARY OF ADJUSTING THIS STUFF AFTER THIS WORKING PERFECTLY NOW.
public:
    void update(std::vector<std::unique_ptr<Entity> > &entities) {
        for (auto &e: entities) {
            if (e->hasComponent<ScaleThisObjectByDistance>() &&
                e->hasComponent<Transform>() &&
                e->hasComponent<Sprite>()) {
                auto &transform = e->getComponent<Transform>();
                auto &sprite = e->getComponent<Sprite>();


                // Get ratio for scaling
                float depthMin = 127.24f; // literal vertical screen pixels (at bottom of screen), min value for groundY
                float depthMax = 526.24f; // max value for groundY
                float depthRatio = 1.0f - std::clamp((transform.groundY - depthMin) / (depthMax - depthMin), 0.0f, 1.0f);

                float heightMinAtMaxDepth = 202.095; // Height Above Ground: 568.418 from debug at max depth
                float heightMaxAtMinDepth = 568.418; // Height Above Ground: 202.095 from debug at min depth

                // old, without dept consideration
                float heightMin = transform.groundY;
                float expectedHeightAtCurrentDepth = heightMinAtMaxDepth +
                                                     (heightMaxAtMinDepth - heightMinAtMaxDepth) * (depthRatio);

                float heightAboveGround = transform.groundY - transform.position.y;
                // float heightRatio = 1.0f - std::clamp((heightAboveGround - heightMin) / (heightMaxAtMinDepth - heightMin), 0.0f,
                //                                       1.0f);
                float heightRatio = 1.0f - std::clamp(
                                        (heightAboveGround - heightMinAtMaxDepth) / (
                                            heightMaxAtMinDepth - heightMinAtMaxDepth),
                                        0.0f, 1.0f
                                    );

                // Blend depth and height (depth dominates, height minor) - camera perspective is tilted down and forward 45 degrees
                float finalRatio = depthRatio + heightRatio * 0.1f;
                finalRatio = std::clamp(finalRatio, 0.0f, 1.0f);

                // new scale
                sprite.currentScale = sprite.maxScale - finalRatio * (sprite.maxScale - sprite.minScale);

                float scaledWidth = sprite.baseWidth * sprite.currentScale;
                float scaledHeight = sprite.baseHeight * sprite.currentScale;

                if (e->hasComponent<Gun>()) {
                    auto &gun = e->getComponent<Gun>();
                    gun.gunTip1OffsetXScaled = gun.gunTip1BaseOffsetX * sprite.currentScale;
                    gun.gunTip1OffsetYScaled = gun.gunTip1BaseOffsetY * sprite.currentScale;
                    gun.gunTip2OffsetXScaled = gun.gunTip2BaseOffsetX * sprite.currentScale;
                    gun.gunTip2OffsetYScaled = gun.gunTip2BaseOffsetY * sprite.currentScale;
                }

                if (e->hasComponent<HitBox>()) {
                    auto &hitBox = e->getComponent<HitBox>();
                    // Compute the scaled hitbox values from base values:
                    hitBox.hitBox1widthScaled = hitBox.hitBox1baseWidth * sprite.currentScale;
                    hitBox.hitBox1heightScaled = hitBox.hitBox1baseHeight * sprite.currentScale;

                    hitBox.hitBox1offsetXScaled = hitBox.hitBox1baseOffsetX * sprite.currentScale + (
                                                sprite.baseWidth - scaledWidth) / 2.0f;
                    hitBox.hitBox1offsetYScaled = hitBox.hitBox1baseOffsetY * sprite.currentScale + (
                                                sprite.baseHeight - scaledHeight);

                    hitBox.hitBox2offsetXScaled = hitBox.hitBox2baseOffsetX * sprite.currentScale + (
                                                sprite.baseWidth - scaledWidth) / 2.0f;
                    hitBox.hitBox2offsetYScaled = hitBox.hitBox2baseOffsetY * sprite.currentScale + (
                                                sprite.baseHeight - scaledHeight);
                }


                // Anchor bottom and center horizontally
                // This - will replace some code in render - in render we check if entity has hitbox. FYI
                sprite.dst.w = scaledWidth;
                sprite.dst.h = scaledHeight;
                sprite.dst.x = transform.position.x + (sprite.baseWidth - scaledWidth) / 2.0f;
                sprite.dst.y = transform.position.y + (sprite.baseHeight - scaledHeight);

                // DEBUGGER
                // std::cout << "\n[CharacterScaleSystem Debug]\n";
                // std::cout << "----------------------------------------\n";
                // // std::cout << "Entity ID: " << e->getId() << "\n";
                //
                // // === Transform ===s
                // std::cout << "Transform.position: (" << transform.position.x << ", " << transform.position.y << ")\n";
                // std::cout << "Transform.groundY:  " << transform.groundY << "\n\n";
                //
                // // === Ratios ===
                // std::cout << "Depth Ratio:  " << depthRatio << "\n";
                // std::cout << "Height Above Ground: " << heightAboveGround << "\n";
                // std::cout << "Height Ratio: " << heightRatio << "\n";
                // std::cout << "Final Ratio:  " << finalRatio << "\n\n";
                //
                // // === Sprite Scaling ===
                // std::cout << "Scale: " << sprite.currentScale << " (Min: " << sprite.minScale
                //         << ", Max: " << sprite.maxScale << ")\n";
                // std::cout << "Scaled Width:  " << scaledWidth << "\n";
                // std::cout << "Scaled Height: " << scaledHeight << "\n\n";
                //
                // // === Sprite Destination ===
                // std::cout << "Sprite.dst.x: " << sprite.dst.x << "\n";
                // std::cout << "Sprite.dst.y: " << sprite.dst.y << "\n";
                // std::cout << "Sprite.dst.w: " << sprite.dst.w << "\n";
                // std::cout << "Sprite.dst.h: " << sprite.dst.h << "\n\n";
                //
                // // === HitBox 1 ===
                // std::cout << "[HitBox 1]\n";
                // std::cout << "  Base Offset: (" << hitBox.hitBox1baseOffsetX << ", " << hitBox.hitBox1baseOffsetY <<
                //         ")\n";
                // std::cout << "  Base Size:   (" << hitBox.hitBox1baseWidth << " x " << hitBox.hitBox1baseHeight <<
                //         ")\n";
                // std::cout << "  Scaled Offset: (" << hitBox.hitBox1offsetX << ", " << hitBox.hitBox1offsetY << ")\n";
                // std::cout << "  Scaled Size:   (" << hitBox.hitBox1width << " x " << hitBox.hitBox1height << ")\n\n";
                //
                // // === HitBox 2 ===
                // std::cout << "[HitBox 2]\n";
                // std::cout << "  Base Offset: (" << hitBox.hitBox2baseOffsetX << ", " << hitBox.hitBox2baseOffsetY <<
                //         ")\n";
                // std::cout << "  Base Size:   (" << hitBox.hitBox2baseWidth << " x " << hitBox.hitBox2baseHeight <<
                //         ")\n";
                // std::cout << "  Scaled Offset: (" << hitBox.hitBox2offsetX << ", " << hitBox.hitBox2offsetY << ")\n";
                // std::cout << "  Scaled Size:   (" << hitBox.hitBox2width << " x " << hitBox.hitBox2height << ")\n\n";
                //
                // // === Alignment Check ===
                // std::cout << "Bottom Alignment Check: "
                //         << "BottomY=" << (sprite.dst.y + sprite.dst.h)
                //         << " vs GroundY=" << transform.position.y
                //         << " (diff=" << (sprite.dst.y + sprite.dst.h - transform.position.y) << ")\n";
                //
                // std::cout << "----------------------------------------\n";
            }
        }
    }
};

#endif //COMP8552_FINAL_PROJECT_CHARACTERSCALESYSTEM_H
