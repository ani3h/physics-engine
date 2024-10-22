#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>
#include <iostream>

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

    Vector2D& operator-=(const Vector2D& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2D operator/(float scalar) const {
        return Vector2D(x / scalar, y / scalar);
    }

    Vector2D& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Unary minus operator to negate the vector
    Vector2D operator-() const {
        return Vector2D(-x, -y);
    }

    // Length (or magnitude) of the vector
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalize the vector
    Vector2D normalized() const {
        float len = length();
        return len == 0 ? Vector2D(0, 0) : Vector2D(x / len, y / len);
    }

    // Dot product of two vectors
    float dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    // Get the perpendicular vector (useful for certain force applications)
    Vector2D perpendicular() const {
        return Vector2D(-y, x);  // 90 degrees rotation
    }

    // Check if two vectors are equal
    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }

    // Check if two vectors are not equal
    bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }
};

#endif // VECTOR2D_H
