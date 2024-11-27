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
    Vector2D& posA = objectA.getPosition();
    Vector2D& posB = objectB.getPosition();
    Vector2D& velA = objectA.getVelocity();
    Vector2D& velB = objectB.getVelocity();
    float massA = objectA.getMass();
    float massB = objectB.getMass();

    // Calculate overlap in both axes
    float overlapX = (posA.x + width) - posB.x;
    if (std::abs(overlapX) > std::abs(posA.x - (posB.x + width))) {
        overlapX = posA.x - (posB.x + width);
    }
    
    float overlapY = (posA.y + height) - posB.y;
    if (std::abs(overlapY) > std::abs(posA.y - (posB.y + height))) {
        overlapY = posA.y - (posB.y + height);
    }

    // Determine collision normal based on smallest overlap
    Vector2D normal;
    if (std::abs(overlapX) < std::abs(overlapY)) {
        normal = Vector2D(overlapX > 0 ? 1.0f : -1.0f, 0.0f);
    } else {
        normal = Vector2D(0.0f, overlapY > 0 ? 1.0f : -1.0f);
    }

    // Calculate relative velocity
    Vector2D relativeVel = velB - velA;
    float velocityAlongNormal = relativeVel.dot(normal);

    // Early out if objects are moving apart
    if (velocityAlongNormal > 0) return;

    // Calculate restitution (elasticity)
    float e = 0.4f; // Reduced from 0.8f for more realistic block stacking

    // Calculate impulse scalar
    float j = -(1.0f + e) * velocityAlongNormal;
    j /= (1.0f / massA) + (1.0f / massB);

    // Apply impulse
    Vector2D impulse = normal * j;
    velA -= impulse / massA;
    velB += impulse / massB;

    // Apply positional correction to prevent sinking
    const float percent = 0.2f; // Penetration percentage to correct
    const float slop = 0.01f;   // Penetration allowance
    Vector2D correction = normal * (std::max(std::abs(std::abs(overlapX) < std::abs(overlapY) ? 
                                           overlapX : overlapY) - slop, 0.0f) * percent);
    
    posA -= correction * (1.0f / massA / (1.0f / massA + 1.0f / massB));
    posB += correction * (1.0f / massB / (1.0f / massA + 1.0f / massB));

    // Apply friction
    float friction = 0.3f; // Increased from 0.2f for better stability
    Vector2D tangent = relativeVel - (normal * velocityAlongNormal);
    if (tangent.lengthSquared() > 0.0001f) {
        tangent = tangent.normalized();
        float jt = -relativeVel.dot(tangent);
        jt /= (1.0f / massA) + (1.0f / massB);

        // Clamp friction
        Vector2D frictionImpulse;
        if (std::abs(jt) < j * friction) {
            frictionImpulse = tangent * jt;
        } else {
            frictionImpulse = tangent * (-j * friction);
        }

        velA -= frictionImpulse / massA;
        velB += frictionImpulse / massB;
    }

    // Additional stability check for stacked blocks
    if (std::abs(normal.y) > 0.9f) { // Near-vertical collision
        // Reduce horizontal velocity for better stacking stability
        if (std::abs(velA.x) < 0.1f) velA.x = 0;
        if (std::abs(velB.x) < 0.1f) velB.x = 0;
    }
}

// Circle vs Circle collision response
void CircleCollider::resolveCollision(Object& objectA, Object& objectB) const {
    Vector2D& posA = objectA.getPosition();
    Vector2D& posB = objectB.getPosition();
    Vector2D& velA = objectA.getVelocity();
    Vector2D& velB = objectB.getVelocity();
    float massA = objectA.getMass();
    float massB = objectB.getMass();

    // Calculate collision normal
    Vector2D normal = (posB - posA).normalized();
    Vector2D relativeVel = velB - velA;
    
    float velocityAlongNormal = relativeVel.dot(normal);

    // Don't resolve if objects are moving apart
    if (velocityAlongNormal > 0) return;

    // Coefficient of restitution (elasticity)
    float e = 0.8f;

    // Calculate impulse scalar using conservation of momentum and energy
    float j = -(1.0f + e) * velocityAlongNormal;
    j /= (1.0f / massA) + (1.0f / massB);

    // Apply impulse
    Vector2D impulse = normal * j;
    velA -= impulse / massA;
    velB += impulse / massB;
    
    // Apply friction (similar to AABB collision)
    float friction = 0.2f;
    Vector2D tangent = relativeVel - (normal * velocityAlongNormal);
    if (tangent.lengthSquared() > 0.0001f) {
        tangent = tangent.normalized();
        float jt = -relativeVel.dot(tangent);
        jt /= (1.0f / massA) + (1.0f / massB);

        Vector2D frictionImpulse;
        if (std::abs(jt) < j * friction) {
            frictionImpulse = tangent * jt;
        } else {
            frictionImpulse = tangent * (-j * friction);
        }

        velA -= frictionImpulse / massA;
        velB += frictionImpulse / massB;
    }
}
