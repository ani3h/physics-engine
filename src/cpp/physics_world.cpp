#include "physics_world.h"
#include "collider.h"
#include <algorithm>

// Add an object to the physics world
void PhysicsWorld::addObject(Object* object) {
    objects.push_back(object);
}

// Set a custom force for a specific object by its ID
void PhysicsWorld::setCustomForce(int objectID, const Vector2D& force) {
    customForces[objectID] = force;
}

// Get the custom force for a specific object (if it exists)
Vector2D PhysicsWorld::getCustomForceForObject(int objectID) const {
    auto it = customForces.find(objectID);
    if (it != customForces.end()) {
        return it->second;
    }
    return Vector2D(0, 0);  // No custom force, return zero vector
}

// Apply gravity and custom forces to all objects
void PhysicsWorld::applyForces() {
    for (Object* object : objects) {
        // Apply gravity to each object (if it has mass)
        if (object->getMass() != 0) {
            Vector2D gravityForce = gravity * object->getMass();
            object->applyForce(gravityForce);
        }

        // Apply custom user-defined forces
        int objectID = object->getID();  // Use the object's unique ID
        Vector2D customForce = getCustomForceForObject(objectID);

        // If there is a custom force, apply it
        if (customForce.length() > 0) {
            Forces::applyCustomForce(*object, customForce);
        }
    }
}

// Update the world by one step, advancing the simulation
void PhysicsWorld::step(float deltaTime) {
    // Apply forces to all objects
    applyForces();

    // Update object positions based on their velocity and deltaTime
    for (Object* object : objects) {
        object->updatePosition(deltaTime);  // Update the object's position with velocity
    }

    // Handle collision detection and resolution
    handleCollisions();
}

// Collision detection and resolution
void PhysicsWorld::handleCollisions() {
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            Object* objectA = objects[i];
            Object* objectB = objects[j];

            if (detectCollision(objectA, objectB)) {
                resolveCollision(objectA, objectB);
            }
        }
    }
}

// Detects collision between two objects using their colliders
bool PhysicsWorld::detectCollision(Object* objectA, Object* objectB) {
    // Check the collider types for each object
    Collider* colliderA = objectA->getCollider();
    Collider* colliderB = objectB->getCollider();

    if (colliderA && colliderB) {
        // Use the collider to check for collision
        return colliderA->checkCollision(*colliderB);
    }

    return false;
}

// Resolves the collision between two objects
void PhysicsWorld::resolveCollision(Object* objectA, Object* objectB) {
    // Get the colliders for each object
    Collider* colliderA = objectA->getCollider();
    Collider* colliderB = objectB->getCollider();

    if (colliderA && colliderB) {
        // Use the colliders to resolve the collision
        colliderA->resolveCollision(*objectA, *objectB);
    }
}

// Get the list of objects in the world
const std::vector<Object*>& PhysicsWorld::getObjects() const {
    return objects;  // Return the list of objects
}