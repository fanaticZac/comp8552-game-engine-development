//
// Created by user on 2025-11-01.
//


#include "EventResponseSystem.h"
#include <cfloat>
#include <cmath>
#include <cstring>
#include "Game.h"
#include "World.h"

EventResponseSystem::EventResponseSystem(World &world) {
    // subscribe to collision events once, handle all logic inside onCollision
    world.getEventManager().subscribe([this, &world](const BaseEvent &e) {
        if (e.type != EventType::Collision) return;

        const auto &collision = static_cast<const CollisionEvent &>(e); // cast base type to collision type

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
        if (eA->hasComponent<Projectile>() || eB->hasComponent<Projectile>() && !(
                eA->hasComponent<Projectile>() && eB->hasComponent<Projectile>()))

        // std::strcmp(collA.tag.c_str(), "projectile") == 0 || std::strcmp(collB.tag.c_str(), "projectile") == 0)
        {
            Entity *projectile = eA->hasComponent<Projectile>() ? eA : eB;
            Entity *victim = projectile == eA ? eB : eA;
            auto &projectileTransform = projectile->getComponent<Transform>();
            auto &victimTransform = victim->getComponent<Transform>();
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

            // destroy projectile on hit (common behaviour)
            projectile->destroy();
            // Apply damage
            auto &health = victim->getComponent<Health>();
            health.currentHealth -= projectileProperties.damage;

            // Trigger red flash
            health.damageFlash = true;
            health.damageFlashTimer = 0.2f;

            if (victim->hasComponent<Player>()) {
                Game::gameState.playerHealth = health.currentHealth;
            }

            std::cout
                << (victim->hasComponent<Enemy>() ? "Enemy " : "Player ")
                << "health after hit: "
                << health.currentHealth
                << std::endl;

            // Check for death
            if (health.currentHealth <= 0) {
                if (victim->hasComponent<EnemyBehavior>()) {
                    auto &behavior = victim->getComponent<EnemyBehavior>();
                    behavior.shooting = false;
                    behavior.charging = false;
                    behavior.dying = true;
                    behavior.fireState = EnemyBehavior::State::Dying;
                }
                if (victim->hasComponent<Player>()) {
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

    if (c.state == CollisionState::Exit) {
        if ((std::strcmp(collA.tag.c_str(), "player") == 0 && std::strcmp(collB.tag.c_str(), "wall") == 0) ||
            (std::strcmp(collA.tag.c_str(), "wall") == 0 && std::strcmp(collB.tag.c_str(), "player") == 0)) {
            Entity *player = (std::strcmp(collA.tag.c_str(), "player") == 0) ? eA : eB;
            // Just extra to ensure proper animation reset on collision exit.
            auto &playerProperties = player->getComponent<Player>();
            // playerProperties.isStandingOnObstacle = false; // This causes problems with stacked cubes
            playerProperties.isStandingBehindObstacleEntityID = -1;
        }
    }

    if (c.state == CollisionState::Stay || c.state == CollisionState::Enter) {
        if ((std::strcmp(collA.tag.c_str(), "player") == 0 && std::strcmp(collB.tag.c_str(), "wall") == 0) ||
            (std::strcmp(collA.tag.c_str(), "wall") == 0 && std::strcmp(collB.tag.c_str(), "player") == 0)) {
            Entity *player = (std::strcmp(collA.tag.c_str(), "player") == 0) ? eA : eB;
            Entity *wall = (player == eA) ? eB : eA;

            auto &playerTransform = player->getComponent<Transform>();
            auto &wallTransform = wall->getComponent<Transform>();
            auto &playerCollider = player->getComponent<Collider>();
            auto &wallCollider = wall->getComponent<Collider>();
            auto &playerPhysics = player->getComponent<Physics>();
            auto &playerProperties = player->getComponent<Player>();

            // NOTE: If we are here - images are already overlapped.

            // NOTE:: This needs to operate from the ground up. Currently groundY values were arbitrary, and configured to top the top of the sprite
            // NOTE:: However this is problematic with regard to hitboxes and the theay I'm trying to do the cube calculations

            // NOTE:: The problem: I was to disable movement that would intersect the shape of the cube that has angles. We have 2 hitboxe offsect that repesent the front abd back of the box, relative to the sprite, we need to get their world positions, then interpolate the offsets for each pixel, and check against the player sprite
            // NOTE: We already have registered collision.

            std::cout << "Collision" << std::endl;

            //  CHECKPOINT: If not aligned ground.y at the bottom or top(depth) of the cube - we can skip all this.
            // if (playerTransform.groundZ > wallTransform.groundZ) {
            //     std::cout << "COLLISION ERROR: player behind cube ";
            //     return;
            // }
            // if (playerTransform.groundZ < wallTransform.groundZ - wallTransform.depth) {
            //     std::cout << "COLLISION ERROR: player in front of cube ";
            //     return;
            // }
            if (!wallCollider.hitbox3_isActive) {
                std::cout << "COLLISION ERROR: cube hitbox3 failed to be activated";
                return;
            }

            // playerProperties.isStandingOnObstacle = false; // Reset

            // PLAYER
            float HB_scaledAndPositionedNewX = playerTransform.position.x - playerCollider.hitBox1Scaled.x + (
                                                   playerTransform.width - playerCollider.hitBox1Scaled.w) / 2.0f;
            // player horizontal - these just use hb1 because both are same width and offset, boost/not-boost
            float playerHB_Left = HB_scaledAndPositionedNewX + playerCollider.hitBox1Scaled.x;
            float playerHB_Right = HB_scaledAndPositionedNewX + playerCollider.hitBox1Scaled.x + playerCollider.
                                   hitBox1Scaled.w;

            // Player vertical
            float playerHB_Bottom = playerTransform.position.y + playerTransform.height;

            float playerHB_Top = (playerPhysics.grounded)
                                     ? // This does matter which animation
                                     playerTransform.position.y - playerCollider.hitBox1Scaled.y + (
                                         playerTransform.height - playerCollider.hitBox1Scaled.h)
                                     : playerTransform.position.y - playerCollider.hitBox2Scaled.y + (
                                           playerTransform.height - playerCollider.hitBox2Scaled.h);

            // front hitbox aka hitbox (1)
            // float wallHB1_Left = wallTransform.position.x + wallCollider.hitBox1Scaled.x;
            // float wallHB1_Right = wallTransform.position.x + wallCollider.hitBox1Scaled.x + wallCollider.
            //                      hitBox1Scaled.w;
            // float wallHB1_Top = wallTransform.position.y + wallCollider.hitBox1Scaled.y;
            // float wallHB1_Bottom = wallTransform.position.y + wallCollider.hitBox1Scaled.y + wallCollider.
            //                       hitBox1Scaled.h;


            // WORKING - MOVED TO UTILITY FUNCTION createDynamicHitBox3
            // float cubeDepthMaximum = std::fabs(wallCollider.hitBox1Scaled.x - wallCollider.hitBox2Scaled.x);
            // float cubeDepthGroundZ = wallTransform.groundZ - playerTransform.groundZ;
            // float groundZ_alpha = cubeDepthGroundZ / cubeDepthMaximum;
            // groundZ_alpha = std::clamp(groundZ_alpha, 0.0f, 1.0f);
            // float newCubeHitBoxOffsetValue = cubeDepthMaximum * (1.0 - groundZ_alpha);
            //
            // // Hitbox3 is for dynamic debugger
            // wallCollider.hitbox3_isActive = true;
            // wallCollider.hitBox3Scaled.x = newCubeHitBoxOffsetValue;
            // wallCollider.hitBox3Scaled.y = newCubeHitBoxOffsetValue;
            // wallCollider.hitBox3Scaled.w = wallCollider.hitBox2Scaled.w; // same as before we just change position
            // wallCollider.hitBox3Scaled.h = wallCollider.hitBox2Scaled.h; // same as before we just change position
            // if (!wallCollider.createDynamicHitBox3ForObstacles(playerTransform.groundZ, wallTransform.groundZ)) return;


            // NEW HITBOX EDGES
            float wallHB_Left = wallTransform.position.x + wallCollider.hitBox3Scaled.x;
            float wallHB_Right = wallTransform.position.x + wallCollider.hitBox3Scaled.x + wallCollider.
                                 hitBox3Scaled.w;
            float wallHB_Top = wallTransform.position.y + wallCollider.hitBox3Scaled.y;
            float wallHB_Bottom = wallTransform.position.y + wallCollider.hitBox3Scaled.y + wallCollider.
                                  hitBox3Scaled.h;

            //  CHECKPOINT: This is only a collision if the player is not grounded and feet above the top of the new hitbox value
            // if (!playerPhysics.grounded && playerHB_Bottom < wallHB_Top) {std::cout << "player above cube "; return;}

            // OVERLAPS
            float overlap_Left = playerHB_Right - wallHB_Left;
            float overlap_Right = wallHB_Right - playerHB_Left;
            // For top and bottom of player - we only care about bottom of player
            // Distinction needs to be made for groundZ
            float overlap_Top = playerHB_Bottom - wallHB_Top;
            // float overlap_Bottom = wallHB_Bottom - playerHB_Bottom;
            float overlap_Bottom = wallTransform.position.y + wallTransform.height - playerHB_Bottom;
            //HB1 is front of cube. HB3 is always 0 because it matchs depth
            // always 0 - dynamic hitbox aligns with player bottom

            std::cout << "\n--- COLLISION DEBUG ---\n";
            // transforms (player and wall)
            std::cout << "PLAYER position.X: " << playerTransform.position.x << "\nPLAYER position.Y: " <<
                    playerTransform.position.y << "\nPLAYER position.groundZ: " << playerTransform.groundZ << std::endl;
            std::cout << "WALL position.X: " << wallTransform.position.x << "\nWALL position.Y: " << wallTransform.
                    position.y << "\nWALL position.groundZ: " << wallTransform.groundZ << std::endl;

            // player hitbox
            std::cout << "playerHB_Left: " << playerHB_Left << std::endl;
            std::cout << "playerHB_Right: " << playerHB_Right << std::endl;
            std::cout << "playerHB_Top: " << playerHB_Top << std::endl;
            std::cout << "playerHB_Bottom: " << playerHB_Bottom << std::endl;
            // new wall hitbox
            std::cout << "wallHB_Left   = " << wallHB_Left << "\n";
            std::cout << "wallHB_Right   = " << wallHB_Right << "\n";
            std::cout << "wallHB_Top   = " << wallHB_Top << "\n";
            std::cout << "wallHB_Bottom   = " << wallHB_Bottom << "\n";

            std::cout << "overlap_Left   = " << overlap_Left << "\n";
            std::cout << "overlap_Right   = " << overlap_Right << "\n";
            std::cout << "overlap_Top  = " << overlap_Top << "\n";
            std::cout << "overlap_Bottom  = " << overlap_Bottom << "\n";
            std::cout << "-------------------------\n";

            float overlapCornerMargin = 10.0f;

            if (overlap_Top > 0 &&
                // Ensures it doesn't jump up from side
                playerHB_Bottom < wallHB_Top + wallCollider.hitBox3Scaled.h * .20 &&
                // Ensures player can walk to edge
                overlap_Left < wallCollider.hitBox3Scaled.w + playerCollider.hitBox1Scaled.w &&
                // Ensures player can walk to edge
                overlap_Right < wallCollider.hitBox3Scaled.w + playerCollider.hitBox1Scaled.w) {
                playerTransform.position.y -= overlap_Top;
                playerProperties.isStandingOnObstacle = true;
            } else if (overlap_Bottom > 0 && // use front HB for bottom
                       overlap_Bottom < wallCollider.hitBox3Scaled.h * .20 &&
                       (overlap_Left >= overlapCornerMargin && overlap_Right >= overlapCornerMargin)) {
                // Player hit the cube from underneath → push downward
                playerTransform.groundZ += overlap_Bottom;
                if (playerPhysics.grounded) {
                    playerTransform.position.y = playerTransform.groundZ;
                }
                playerProperties.isStandingOnObstacle = false;
            } else {
                playerProperties.isStandingOnObstacle = false;
                if (overlap_Left > 0 && playerHB_Bottom > wallHB_Top + 1.0f && overlap_Right >
                    overlap_Left) {
                    playerTransform.position.x -= overlap_Left;
                }
                if (overlap_Right > 0 && playerHB_Bottom > wallHB_Top + 1.0f && overlap_Left >
                    overlap_Right) {
                    playerTransform.position.x += overlap_Right;
                    playerProperties.isStandingBehindObstacleEntityID = wall->getID();
                }
            }

            std::cout << "DEBUG: overlap_Bottom = " << overlap_Bottom
                    << ", overlap_Left = " << overlap_Left
                    << ", overlap_Right = " << overlap_Right << std::endl;

            // ========================================================================
            //  "YOU SHALL NOT PASS (wall boundaries) !!!! ... .... flee you fool(ish player)s"

            /*,                      _,-
(\                  _,-','
 \\              ,-"  ,'
  \\           ,'   ,'
   \\        _:.----__.-."-._,-._
    \\    .-".:--`:::::.:.:'  )  `-.
     \\   `. ::L .::::::'`-._  (  ) :
      \\    ":::::::'  `-.   `-_ ) ,'
       \\.._/_`:::,' `.     .  `-:
       :" _   "\"" `-_    .    `  `.
        "\\"":--\     `-.__ ` .     `.
          \\'::  \    _-"__`--.__ `  . `.     _,--..-
           \\ ::  \_-":)(        ""-._ ` `.-''
            \\`:`-":::/ \\ .   .      `-.  :
            :\\:::::::'  \\     `    .   `. :
             :\\:':':'  . \\           `,  : :
             : \\     .    \\      .       `. :       ,-
            __`:\\      .   \\ .   `  ,'    ,: :   ,-'
     _,---""  :  \\ '        \\  .          :-"  ,'
 ,-""        :    \\:  .  :   \\  `  '     ,'   /
'            :  :  \       .   \\   .   _,'  ,-'
            :  .   '       :   :`   `,-' ,--'
             :     :   :      ,'-._,' ,-'
             _:     :        : :  ,--'
               `-._,'-._.__-""' ,'
                          ,----'
                   _.----'
           __..--""
         ""*/
            // ========================================================================
        }
    }
}
