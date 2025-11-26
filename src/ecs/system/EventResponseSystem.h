//
// Created by user on 2025-11-01.
//

#ifndef COMP8552_FINAL_PROJECT_EVENTRESPONSESYSTEM_H
#define COMP8552_FINAL_PROJECT_EVENTRESPONSESYSTEM_H

#include <functional>

#include "Component.h"
#include "event/BaseEvent.h"

class Entity;
class World;

class EventResponseSystem {
public:
    EventResponseSystem(World &world);
private:
    // collisions
    void onCollision(const CollisionEvent &c, World &world);

    // bool createDynamicHitBox3(const Transform &otherTransform, const Transform &wallTransform, HitBox &wallHitBox);

    // This pointer passed by reference allows you to change what the pointer points to
    // bool getCollisionEntries(const CollisionEvent& e, const char* otherTag, Entity*& player, Entity*& other);

    // entities
    // void onPlayerAction(const PlayerActionEvent& e, const std::function<void(Entity* player, PlayerAction action)>& callback);
};

#endif //COMP8552_FINAL_PROJECT_EVENTRESPONSESYSTEM_H