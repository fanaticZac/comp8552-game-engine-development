//
// Created by user on 2025-09-26.
//

#include "Vector2D.h"
#include <cmath>
//member operator function so we can use Vector2D on the left
Vector2D Vector2D::operator*(float scaler) const {
    return Vector2D (x * scaler, y * scaler);
}

//non-member operator function to make use of putting vector on right hand side
Vector2D operator*(float scaler, Vector2D& vec) {
    return Vector2D (vec.x * scaler, vec.y * scaler);
}

// member operator function so we can use the vector2D that we want to the use on the left
// most effecient way - we're not creating a copy
Vector2D& Vector2D::operator+=(const Vector2D &vec) {
    this->x += vec.x;
    this->y += vec.y;
    return *this;
}


Vector2D& Vector2D::normalize() {
    float length = sqrt(x * x + y * y);

    if (length > 0) {
        this->x /= length;
        this->y /= length;
    }

    return *this;
}