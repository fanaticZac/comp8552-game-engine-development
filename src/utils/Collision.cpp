//
// Created by user on 2025-10-04.
//

   // static bool AABB(const SDL_FRect& rectA, const SDL_FRect& rectB);
   //  static bool AABB(const Collider& colA, const Collider& colB);

#include "Collision.h"

// Axis Aligned Bounding Box = AABB
// passing in rect
bool Collision::AABB(const SDL_FRect& rectA, const SDL_FRect& rectB) {
    // 1. Is the right edge of rectangle A >= the left edge of rectangle B.
    // 2. Is the right edge of rectangle B >= the left edge of rectangle A.
    // 3. Is the bottom edge of rectangle A >= the top edge of rectangle B.
    // 4. Is the bottom edge of rectangle B >= the top edge of rectangle A.

    if (rectA.x + rectA.w >= rectB.x &&
       rectB.x + rectB.w >= rectA.x &&
       rectA.y + rectA.h >= rectB.y &&
       rectB.y + rectB.h >= rectA.y
       ) {
        return true;
       }

    return false;
}

// passing in collider itself
// --- Don't use anymore - multiple purposed hitboxes, non standard.
// bool Collision::AABB(const Collider& colA, const Collider& colB) {
//     if (AABB(colA.hitBox1Scaled, colB.hitBox1Scaled)) {
//         return true;
//     }
//     return false;
// }