//
// Created by user on 2025-11-01.
//


#include "EventResponseSystem.h"

#include "Game.h"
#include "World.h"

EventResponseSystem::EventResponseSystem(World &world) {
    // subscriptions
    world.getEventManager().subscribe([this, &world](const BaseEvent &e) {
        if (e.type != EventType::Collision) return;

        const auto &collision = static_cast<const CollisionEvent&>(e); // cast base type to collision type

        onCollision(collision, "item", world);
        onCollision(collision, "wall", world);
        onCollision(collision, "projectile", world);
    });

    world.getEventManager().subscribe([this, &world](const BaseEvent &e) {
        if (e.type != EventType::PlayerAction) return;

        const auto& playerAction = static_cast<const PlayerActionEvent&>(e);

        // TODO onPlayerAction
    });
}

void EventResponseSystem::onCollision(const CollisionEvent &c, const char *otherTag, World &world) {
    Entity *player = nullptr;
    Entity *other = nullptr;

    if (!getCollisionEntries(c, otherTag, player, other)) return;

    if (std::string(otherTag) == "item") {

        if (c.state != CollisionState::Enter) return;

        other->destroy();

        for (auto &entity: world.getEntities()) {
            if (!entity->hasComponent<SceneState>()) continue;
            // Scene state
            auto &sceneState = entity->getComponent<SceneState>();
            sceneState.coinsCollected++;
            if (sceneState.coinsCollected > 1) {
                Game::onSceneChangeRequest("level2");
            }
        }
    } else if (std::string(otherTag) == "wall") {
        //stop the player
        if (c.state != CollisionState::Stay) return;

        auto &t = player->getComponent<Transform>();
        t.position = t.oldPosition;
    } else if (std::string(otherTag) == "projectile") {
        // THIS SHOULD BE DONE WITH A HEALTH COMPONENT VIA UPDATE IS PROPER.
        if (c.state != CollisionState::Enter) return;

        auto& health = player->getComponent<Health>();
        health.currentHealth--;

        Game::gameState.playerHealth = health.currentHealth;

        std::cout << health.currentHealth << std::endl;

        if (health.currentHealth <= 0) {
            player->destroy();
            // change Scene at every end of game loop
            Game::onSceneChangeRequest("gameover");
        }



    }
}

bool EventResponseSystem::getCollisionEntries(
    const CollisionEvent &c,
    const char *otherTag,
    Entity *&player,
    Entity *&other
    ) {

        if (c.entityA == nullptr || c.entityB == nullptr) return false;

        if (!(c.entityA->hasComponent<Collider>() && c.entityB->hasComponent<Collider>())) return false;

        auto& colliderA = c.entityA->getComponent<Collider>();
        auto& colliderB = c.entityB->getComponent<Collider>();

        if (colliderA.tag == "player" && colliderB.tag == otherTag) {
            player = c.entityA;
            other = c.entityB;
        } else if (colliderA.tag == otherTag && colliderB.tag == "player") {
            player = c.entityB;
            other = c.entityA;
        }

        return player && other;
}
