//
// Created by user on 2025-09-26.
//

#ifndef COMP8552_FINAL_PROJECT_VECTOR2D_H
#define COMP8552_FINAL_PROJECT_VECTOR2D_H

class Vector2D {
public:
    float x = 0.0f;
    float y = 0.0f;

    Vector2D() {
        x = 0.0f;
        y = 0.0f;
    }

    Vector2D(float x, float y): x(x), y(y) {}

    // member operator function
    // Vector2D has to be on the left, float on the right
    Vector2D operator*(float scaler) const;
    // float on the left, Vector2D has to be on the right
    // Friend makes it non-member function by-pass limitation (for scope)
    friend Vector2D operator*(float scaler, Vector2D& vec);

    // Vector we want to return on the left,
    // another vector on the right
    Vector2D& operator+=(const Vector2D& vec);

    Vector2D& normalize();

};

#endif //COMP8552_FINAL_PROJECT_VECTOR2D_H