#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

class Vector2D {
public:
    float x, y;

    // Constructors
    Vector2D() : x(0), y(0) {}
    Vector2D(float x, float y) : x(x), y(y) {}

    // Operator overloads for vector arithmetic
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D operator/(float scalar) const {
        return Vector2D(x / scalar, y / scalar);
    }

    // Magnitude of the vector
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalize the vector
    Vector2D normalized() const {
        float mag = magnitude();
        return mag == 0 ? Vector2D(0, 0) : Vector2D(x / mag, y / mag);
    }
};

#endif // VECTOR2D_H
