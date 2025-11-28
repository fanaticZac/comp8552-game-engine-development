//
// Created by user on 2025-10-14.
//

#ifndef COMP8552_FINAL_PROJECT_ANIMATIONSYSTEM_H
#define COMP8552_FINAL_PROJECT_ANIMATIONSYSTEM_H

#include <memory>
#include <vector>


#include "Component.h"
#include "Entity.h"

//State system: Deciding which clip to use
//Check if the animation has been switched
//Playback system: advance the animation

class AnimationSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity> > &entities, float dt) {
        for (auto &e: entities) {
            if (e->hasComponent<Animation>() &&
                e->hasComponent<Transform>()
            ) {
                auto &animation = e->getComponent<Animation>();
                auto &transform = e->getComponent<Transform>();


                // State system
                std::string newClip;

                // NOTE: lower is faster : seconds before switching frame
                // NOTE: meaning your timing will need to match for certain actions
                // Player Animation Speeds
                float walkSpeed = 0.08f; // same as idle (run default) - should match level speed
                float walkBackwardsSpeed = 0.1f; // same as idle (run default) - should match level speed
                float walkVerticallySpeed = 0.3f;
                float runSpeed = 0.08f;
                float runAndGunSpeed = 0.09f;
                float runFastSpeed = 0.06f;
                float runAndGunFastSpeed = 0.08f;
                float boostSpeed = 0.04f;
                float fallSpeed = 0.5;

                // Enemy1 Animation Speeds
                float enemy1IdleSpeed = 1.0f;
                float enemy1ChargingSpeed = 0.5f;
                float enemy1ShootingSpeed = 1.0f;
                float enemy1DyingSpeed = 3.0f / 6; // 2 seconds total


                if (e->hasComponent<Player>() &&
                    e->hasComponent<Input>() &&
                    e->hasComponent<Physics>() &&
                    e->hasComponent<Boost>() &&
                    e->hasComponent<Gun>()) {
                    auto &input = e->getComponent<Input>();
                    auto &gun = e->getComponent<Gun>();
                    auto &boost = e->getComponent<Boost>();
                    auto &physics = e->getComponent<Physics>();
                    auto &playerProperties = e->getComponent<Player>();

                    if (boost.boosting) {
                        newClip = "boost";
                        if (gun.gun1Shooting) newClip = "boost_and_gun_0";
                        if (gun.gun2Shooting) newClip = "boost_and_gun_1";

                        animation.speed = boostSpeed;
                    } else if (!physics.grounded && !playerProperties.isStandingOnObstacle) {
                        newClip = "fall";
                        animation.speed = fallSpeed;
                        if (gun.gun1Shooting) newClip = "fall_and_gun_0";
                        if (gun.gun2Shooting) newClip = "fall_and_gun_1";
                    } else {
                        if (gun.gun1Shooting) {
                            if (input.moveRight) {
                                newClip = "run_and_gun_0";
                                animation.speed = runAndGunSpeed;

                                if (!input.moveTop && !input.moveBottom) {
                                    // When going diagonal run faster/**/
                                    newClip = "run_and_gun_0_fast";
                                    animation.speed = runAndGunFastSpeed;
                                }
                            } else if (input.moveRight && input.moveLeft && transform.groundZ != transform.oldGroundZ) {
                                // Moving up and down
                                newClip = "walk_and_gun_0";
                                animation.speed = walkVerticallySpeed;
                            } else if (input.moveLeft && transform.groundZ == transform.oldGroundZ) {
                                if (input.moveBackwards) {
                                    newClip = "walk_and_gun_1";
                                animation.speed = walkBackwardsSpeed;
                                }else {
                                    newClip = "stand_and_gun_0";
                                animation.speed = 5.0f; // it's only 1 clip
                                }
                            } else {
                                newClip = "walk_and_gun_0";
                                animation.speed = walkSpeed;
                            }
                        } else if (gun.gun2Shooting) {
                            if (input.moveRight) {
                                newClip = "run_and_gun_1";
                                animation.speed = runAndGunSpeed;

                                if (!input.moveTop && !input.moveBottom) {
                                    // When going diagonal run faster/**/
                                    newClip = "run_and_gun_1_fast";
                                    animation.speed = runAndGunFastSpeed;
                                }
                            } else if (input.moveRight && input.moveLeft && transform.groundZ != transform.oldGroundZ) {
                                // Moving up and down
                                newClip = "walk_and_gun_1";
                                animation.speed = walkVerticallySpeed;
                            } else if (input.moveLeft && transform.groundZ == transform.oldGroundZ) {
                                if (input.moveBackwards) {
                                    newClip = "walk_and_gun_1";
                                animation.speed = walkBackwardsSpeed;
                                } else {
                                    newClip = "stand_and_gun_1";
                                    animation.speed = 5.0f; // it's only 1 clip
                                }

                            } else {
                                newClip = "walk_and_gun_1";
                                animation.speed = walkSpeed;
                            }
                        } else {
                            if (input.moveRight) {
                                newClip = "run";
                                animation.speed = runSpeed;

                                if (!input.moveTop && !input.moveBottom) {
                                    // When going diagonal run faster/**/
                                    newClip = "run_fast";
                                    animation.speed = runFastSpeed;
                                }
                            } else if (input.moveRight && input.moveLeft && transform.groundZ != transform.oldGroundZ) {
                                // Moving up and down
                                newClip = "walk";
                                animation.speed = walkVerticallySpeed;
                            } else if (input.moveLeft && transform.groundZ == transform.oldGroundZ) {
                                if (input.moveBackwards) {
                                    newClip = "walk";
                                    animation.speed = walkBackwardsSpeed;
                                } else {
                                    newClip = "stand";
                                    animation.speed = 5.0f;
                                }
                            } else {
                                newClip = "walk";
                                animation.speed = walkSpeed;
                            }
                        }
                    }
                } else if (e->hasComponent<Enemy>() &&
                           e->hasComponent<EnemyBehavior>()) {
                    // Is enemy
                    auto &enemyBehavior = e->getComponent<EnemyBehavior>();
                    if (enemyBehavior.dying) {
                        newClip = "dying";
                        animation.speed = enemy1DyingSpeed;
                    } else if (enemyBehavior.shooting) {
                        newClip = "shooting";
                        animation.speed = enemy1ShootingSpeed;
                    } else if (enemyBehavior.charging) {
                        newClip = "charging";
                        animation.speed = enemy1ChargingSpeed;
                    } else {
                        newClip = "idle";
                        animation.speed = enemy1IdleSpeed;
                    }



                }


                // Check if animation has switched
                // if the chosen clip is different from the current one, switch to new clip, reset time and frame index
                if (newClip != animation.currentClip) {
                    animation.currentClip = newClip; // switch to new clip
                    animation.time = 0.0f;
                    animation.currentFrame = 0;

                    if (e->hasComponent<Player>()) {
                        // This allows me to traverse to the same frame index on the new animations and
                        // -- handle some animations having 4 frames ands some 8. i.e. when running but raising weapon
                        size_t oldFrameCount = animation.clips[animation.currentClip].frameIndices.size();
                        size_t newFrameCount = animation.clips[newClip].frameIndices.size();

                        if (oldFrameCount > 0 && newFrameCount > 0) {
                            float progress = static_cast<float>(animation.currentFrame) / oldFrameCount;
                            float newFrameFloat = progress * newFrameCount;

                            // Take next frame after conversion.
                            animation.currentFrame = static_cast<int>(newFrameFloat) % newFrameCount;
                        } else {
                            animation.currentFrame = 0; // fallback
                        }
                    }
                }

                // playback system: advance the animation;
                float animFrameSpeed = animation.speed;
                // how long each 'aniation frame' should last (opposed to game loop)
                auto &clip = animation.clips[animation.currentClip];
                // retrieve the frame data from the current clip
                // advance time
                animation.time += dt; // every game loop frame, we will add the accumulated time
                // once enough time has passed
                if (animation.time >= animFrameSpeed) {
                    animation.time -= animFrameSpeed;
                    //subtract animFrameSpeed (e.g. 0.1f) so the extra time isn't lost

                    std::size_t totalAnimationFrames = clip.frameIndices.size();
                    //advance to the next frame (currentAnimationFrame + 1)
                    // wrap around % so it loops when reaching the end of the clip
                    animation.currentFrame = (animation.currentFrame + 1) % totalAnimationFrames;
                }
            }
        }
    }
};

#endif //COMP8552_FINAL_PROJECT_ANIMATIONSYSTEM_H
