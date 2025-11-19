//
// Created by user on 2025-11-01.
//


#include "EventResponseSystem.h"

#include <cstring>

#include "Game.h"
#include "World.h"

EventResponseSystem::EventResponseSystem(World &world) {
    // subscribe to collision events once, handle all logic inside onCollision
    world.getEventManager().subscribe([this, &world](const BaseEvent &e) {
        if (e.type != EventType::Collision) return;

        const auto &collision = static_cast<const CollisionEvent &>(e); // cast base type to collision type

        // onCollision(collision, "item", world);
        // onCollision(collision, "wall", world);
        // onCollision(collision, "projectile", world);
        // onCollision(collision, "enemy", world);

        onCollision(collision, world);
    });

    world.getEventManager().subscribe([this, &world](const BaseEvent &e) {
        if (e.type != EventType::PlayerAction) return;

        const auto &playerAction = static_cast<const PlayerActionEvent &>(e);

        // TODO onPlayerAction
    });
}

// NOTE:: This is adjusted to not be based solely on player collisions (aka laser projectile and enemy are valid and enemy has health system)
void EventResponseSystem::onCollision(const CollisionEvent &c, World &world) {
    if (c.entityA == nullptr || c.entityB == nullptr) return;
    if (!c.entityA->hasComponent<Collider>() || !c.entityB->hasComponent<Collider>()) return;

    Entity *eA = c.entityA;
    Entity *eB = c.entityB;
    auto &collA = eA->getComponent<Collider>();
    auto &collB = eB->getComponent<Collider>();

    // ITEM PICKUP: item + player (only on Enter)
    if (c.state == CollisionState::Enter) {
        // detect item by component or tag
        // if (eA->hasComponent<ItemTag>() || std::strcmp(collA.tag.c_str(), "item") == 0) {
        //     Entity* item   = eA->hasComponent<ItemTag>() ? eA : eB;
        //     Entity* picker = item == eA ? eB : eA;
        //
        //     if (picker->hasComponent<PlayerTag>()) {
        //         item->destroy();
        //         for (auto &entity: world.getEntities()) {
        //             if (!entity->hasComponent<SceneState>()) continue;
        //             auto &sceneState = entity->getComponent<SceneState>();
        //             sceneState.coinsCollected++;
        //             if (sceneState.coinsCollected > 1) {
        //                 Game::onSceneChangeRequest("level2");
        //             }
        //         }
        //     }
        //     // item handled; return
        //     return;
        // }

        // Projectile collision with player or entity.
        if (eA->hasComponent<Projectile>() || eB->hasComponent<Projectile>() ||
            std::strcmp(collA.tag.c_str(), "projectile") == 0 || std::strcmp(collB.tag.c_str(), "projectile") == 0) {
            Entity* projectile = eA->hasComponent<Projectile>() ? eA : eB;
            Entity* victim = projectile == eA ? eB : eA;

            auto &projectileProperties = projectile->getComponent<Projectile>();

            // Prevent self hits
            if (projectileProperties.ownerID == victim->getID()) {
                return;
            }

            std::cout << "Projectile ownerID: " << projectileProperties.ownerID
          << " | Victim ID: " << victim->getID() << std::endl;

            // Only apply damage to entities that have Health
            if (!victim->hasComponent<Health>()) {
                // destroy projectile on hitting walls/objects
                if (!projectileProperties.piercing) {
                    projectile->destroy();
                }
                return;
            }

            auto &health = victim->getComponent<Health>();
            health.currentHealth = health.currentHealth - projectileProperties.damage;

            // destroy projectile on hit (common behaviour)
            projectile->destroy();

            // update global player UI state if the victim is the player
            if (victim->hasComponent<PlayerTag>()) {
                Game::gameState.playerHealth = health.currentHealth;
            }

            std::cout
                << (victim->hasComponent<Enemy>() ? "Enemy " : "Player ")
                << "health after hit: "
                << health.currentHealth
                << std::endl;

            if (health.currentHealth <= 0) {
                if (victim->hasComponent<EnemyBehavior>()) {
                    auto &behavior = victim->getComponent<EnemyBehavior>();
                    behavior.shooting = false;
                    behavior.charging = false;
                    behavior.dying    = true;
                    behavior.fireState = EnemyBehavior::State::Dying; // NEW: switch state machine
                }

                if (victim->hasComponent<PlayerTag>()) {
                    Game::onSceneChangeRequest("gameover");
                }
            }
            return;
        }

        // Enemy and Player collide
        if ((std::strcmp(collA.tag.c_str(), "player") == 0 && std::strcmp(collB.tag.c_str(), "enemy") == 0) ||
            (std::strcmp(collA.tag.c_str(), "enemy") == 0 && std::strcmp(collB.tag.c_str(), "player") == 0)) {
            // Decide which is the player and which is the enemy
            Entity *player = (std::strcmp(collA.tag.c_str(), "player") == 0) ? eA : eB;
            Entity *enemy = (player == eA) ? eB : eA;

            // Player must have health to take damage
            if (!player->hasComponent<Health>()) {
                return;
            }

            auto &playerHealth = player->getComponent<Health>();

            // Fixed enemy-body damage amount (change if needed)

            playerHealth.currentHealth -= enemy->getComponent<Enemy>().playerCollisionDamage;
            Game::gameState.playerHealth = playerHealth.currentHealth;

            std::cout << "Player damaged by enemy collision. New health: "
                    << playerHealth.currentHealth << std::endl;

            // If player died
            if (playerHealth.currentHealth <= 0) {
                Game::onSceneChangeRequest("gameover");
                return;
            }

            return;
        }
    }

    // WALL BLOCKING: check Stay and rollback mover to old position
    // TODO: Uses old logic - this will need to be reworked after obstacles brought in
    if (c.state == CollisionState::Stay) {
        if (std::strcmp(collA.tag.c_str(), "wall") == 0 || std::strcmp(collB.tag.c_str(), "wall") == 0) {
            Entity *wall = (std::strcmp(collA.tag.c_str(), "wall") == 0) ? eA : eB;
            Entity *mover = wall == eA ? eB : eA;

            if (!mover->hasComponent<Transform>()) return;
            auto &t = mover->getComponent<Transform>();
            t.position = t.oldPosition;
            return;
        }
    }


    // OLD VERSION
    // Entity *player = nullptr;
    // Entity *other = nullptr;
    //
    // if (!getCollisionEntries(c, otherTag, player, other)) return;
    //
    // if (std::string(otherTag) == "item") {
    //
    //     if (c.state != CollisionState::Enter) return;
    //
    //     other->destroy();
    //
    //     for (auto &entity: world.getEntities()) {
    //         if (!entity->hasComponent<SceneState>()) continue;
    //         // Scene state
    //         auto &sceneState = entity->getComponent<SceneState>();
    //         sceneState.coinsCollected++;
    //         if (sceneState.coinsCollected > 1) {
    //             Game::onSceneChangeRequest("level2");
    //         }
    //     }
    // } else if (std::string(otherTag) == "wall") {
    //     //stop the player
    //     if (c.state != CollisionState::Stay) return;
    //
    //     auto &t = player->getComponent<Transform>();
    //     t.position = t.oldPosition;
    // } else if (std::string(otherTag) == "projectile") {
    //     // THIS SHOULD BE DONE WITH A HEALTH COMPONENT VIA UPDATE IS PROPER.
    //     if (c.state != CollisionState::Enter) return;
    //
    //     auto& health = player->getComponent<Health>();
    //     health.currentHealth--;
    //
    //     Game::gameState.playerHealth = health.currentHealth;
    //
    //     std::cout << health.currentHealth << std::endl;
    //
    //     if (health.currentHealth <= 0) {
    //         player->destroy();
    //         // change Scene at every end of game loop
    //         Game::onSceneChangeRequest("gameover");
    //     }
}

//
// bool EventResponseSystem::getCollisionEntries(
//     const CollisionEvent &c,
//     const char *otherTag,
//     Entity *&player,
//     Entity *&other
// ) {
//     if (c.entityA == nullptr || c.entityB == nullptr) return false;
//
//     if (!(c.entityA->hasComponent<Collider>() && c.entityB->hasComponent<Collider>())) return false;
//
//     auto &colliderA = c.entityA->getComponent<Collider>();
//     auto &colliderB = c.entityB->getComponent<Collider>();
//
//     if (colliderA.tag == "player" && colliderB.tag == otherTag) {
//         player = c.entityA;
//         other = c.entityB;
//     } else if (colliderA.tag == otherTag && colliderB.tag == "player") {
//         player = c.entityB;
//         other = c.entityA;
//     }
//
//     return player && other;
// }
