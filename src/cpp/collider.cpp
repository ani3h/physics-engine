#include "collider.h"
#include "object.h"

// AABB vs AABB collision detection
bool checkAABBCollision(const AABBCollider& a, const AABBCollider& b) {
    bool xOverlap = a.position.x < b.position.x + b.width && a.position.x + a.width > b.position.x;
    bool yOverlap = a.position.y < b.position.y + b.height && a.position.y + a.height > b.position.y;
    return xOverlap && yOverlap;
}

// Circle vs Circle collision detection
bool checkCircleCollision(const CircleCollider& a, const CircleCollider& b) {
    float distance = (a.center - b.center).length();
    return distance < (a.radius + b.radius);
}

// AABB vs Circle collision detection
bool checkAABBCircleCollision(const AABBCollider& a, const CircleCollider& b) {
    float closestX = std::max(a.position.x, std::min(b.center.x, a.position.x + a.width));
    float closestY = std::max(a.position.y, std::min(b.center.y, a.position.y + a.height));

    float distance = Vector2D(closestX, closestY).distance(b.center);
    return distance < b.radius;
}

// AABBCollider collision checking
bool AABBCollider::checkCollision(const Collider& other) const {
    const AABBCollider* aabbOther = dynamic_cast<const AABBCollider*>(&other);
    if (aabbOther) return checkAABBCollision(*this, *aabbOther);

    const CircleCollider* circleOther = dynamic_cast<const CircleCollider*>(&other);
    if (circleOther) return checkAABBCircleCollision(*this, *circleOther);

    return false;
}

// CircleCollider collision checking
bool CircleCollider::checkCollision(const Collider& other) const {
    const CircleCollider* circleOther = dynamic_cast<const CircleCollider*>(&other);
    if (circleOther) return checkCircleCollision(*this, *circleOther);

    const AABBCollider* aabbOther = dynamic_cast<const AABBCollider*>(&other);
    if (aabbOther) return checkAABBCircleCollision(*aabbOther, *this);

    return false;
}

// AABB vs AABB collision response
void AABBCollider::resolveCollision(Object& objectA, Object& objectB) const {
    Vector2D& velocityA = objectA.getVelocity();  // Get modifiable reference
    Vector2D& velocityB = objectB.getVelocity();  // Get modifiable reference

    Vector2D relativeVelocity = velocityB - velocityA;

    if (relativeVelocity.x != 0) {
        velocityA.x = -velocityA.x * 0.8f;  // Apply bounce with damping
        velocityB.x = -velocityB.x * 0.8f;
    }
    
    if (relativeVelocity.y != 0) {
        velocityA.y = -velocityA.y * 0.8f;  // Apply bounce with damping
        velocityB.y = -velocityB.y * 0.8f;
    }
}

// Circle vs Circle collision response
void CircleCollider::resolveCollision(Object& objectA, Object& objectB) const {
    Vector2D normal = (objectB.getPosition() - objectA.getPosition()).normalized();
    Vector2D& velocityA = objectA.getVelocity();  // Get modifiable reference
    Vector2D& velocityB = objectB.getVelocity();  // Get modifiable reference

    float relativeVelocity = (velocityB - velocityA).dot(normal);

    if (relativeVelocity > 0) return;  // Already moving apart

    float e = 0.8f;  // Coefficient of restitution for inelastic collision
    float impulse = (-(1 + e) * relativeVelocity) / (1 / objectA.getMass() + 1 / objectB.getMass());

    Vector2D impulseVec = normal * impulse;

    velocityA -= impulseVec / objectA.getMass();
    velocityB += impulseVec / objectB.getMass();
}
