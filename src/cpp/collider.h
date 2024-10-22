#ifndef COLLIDER_H
#define COLLIDER_H

#include "vector2d.h"

class Object;  // Forward declaration for Object class

// Base Collider class
class Collider {
public:
    virtual bool checkCollision(const Collider& other) const = 0;
    virtual void resolveCollision(Object& objectA, Object& objectB) const = 0;
};

// AABB Collider class for rectangular/box-like objects
class AABBCollider : public Collider {
public:
    Vector2D position;  // Top-left corner of the rectangle
    float width, height;

    AABBCollider(const Vector2D& pos, float w, float h) : position(pos), width(w), height(h) {}

    // Check if this AABB collider overlaps with another collider
    bool checkCollision(const Collider& other) const override;

    // Resolve collision with another object (specific to AABB vs. other types)
    void resolveCollision(Object& objectA, Object& objectB) const override;
};

// Circle Collider class for round/spherical objects
class CircleCollider : public Collider {
public:
    Vector2D center;  // Center of the circle
    float radius;

    CircleCollider(const Vector2D& c, float r) : center(c), radius(r) {}

    // Check if this Circle collider overlaps with another collider
    bool checkCollision(const Collider& other) const override;

    // Resolve collision with another object (specific to Circle vs. other types)
    void resolveCollision(Object& objectA, Object& objectB) const override;
};

// Utility functions for collision detection
bool checkAABBCollision(const AABBCollider& a, const AABBCollider& b);
bool checkCircleCollision(const CircleCollider& a, const CircleCollider& b);
bool checkAABBCircleCollision(const AABBCollider& a, const CircleCollider& b);

#endif // COLLIDER_H

