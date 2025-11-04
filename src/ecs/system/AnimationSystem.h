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
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& e : entities) {
            if (e->hasComponent<Animation>() && e->hasComponent<Velocity>()) {
                auto& anim = e->getComponent<Animation>();
                auto& velocity = e->getComponent<Velocity>();

                // State system
                std::string newClip;

                if (e->hasComponent<PlayerTag>()) {
                    if (velocity.direction.x > 0.0f) {
                        newClip = "walk_right";
                    } else if (velocity.direction.x < 0.0f) {
                        newClip = "walk_left";
                    } else if (velocity.direction.y > 0.0f) {
                        newClip = "walk_down"; // swapped
                    } else if (velocity.direction.y < 0.0f) {
                        newClip = "walk_up";   // swapped
                    } else {
                        newClip = "idle";
                    }
                }else {
                    newClip = "idle_right";
                }



                // Check if animation has switched
                // if the chosen clip is different from the current one, switch to new clip, reset time and frame index
                if (newClip != anim.currentClip) {
                    anim.currentClip = newClip; // switch to new clip
                    anim.time = 0.0f;
                    anim.currentFrame = 0;
                }

                // playback system: advance the animation;
                float animFrameSpeed = anim.speed; // how long each 'aniation frame' should last (opposed to game loop)
                auto& clip = anim.clips[anim.currentClip]; // retrieve the frame data from the current clip
                // advance time
                anim.time += dt; // every game loop frame, we will add the accumulated time
                // once enough time has passed
                if (anim.time >= animFrameSpeed) {
                    anim.time -= animFrameSpeed; //subtract animFrameSpeed (e.g. 0.1f) so the extra time isn't lost

                    std::size_t totalAnimationFrames = clip.frameIndices.size();
                    //advance to the next frame (currentAnimationFrame + 1)
                    // wrap around % so it loops when reaching the end of the clip
                    anim.currentFrame = (anim.currentFrame + 1) % totalAnimationFrames;
                }


            }
        }
    }

};

#endif //COMP8552_FINAL_PROJECT_ANIMATIONSYSTEM_H