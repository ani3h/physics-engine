#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <vector>
#include <unordered_map>
#include "object.h"
#include "forces.h"
#include "vector2d.h"
#include "collider.h"

class PhysicsWorld {
public:
    // Add an object to the physics world
    void addObject(Object* object);

    // Set a custom force for a specific object by its ID
    void setCustomForce(int objectID, const Vector2D& force);

    // Get the custom force for a specific object (if it exists)
    Vector2D getCustomForceForObject(int objectID) const;

    // Apply gravity and custom forces to all objects
    void applyForces();

    // Update the world by one step, advancing the simulation
    void step(float deltaTime);

    // Function to get all objects in the physics world
    const std::vector<Object*>& getObjects() const;

private:
    // List of objects in the physics world
    std::vector<Object*> objects;

    // Mapping from object IDs to custom forces
    std::unordered_map<int, Vector2D> customForces;

    // Gravity vector applied to all objects
    Vector2D gravity = Vector2D(0, -9.81f);  // Default gravity, can be customized

    // Handle collision detection and resolution
    void handleCollisions();

    // Detects collision between two objects
    bool detectCollision(Object* objectA, Object* objectB);

    // Resolves the collision between two objects
    void resolveCollision(Object* objectA, Object* objectB);
};

#endif // PHYSICS_WORLD_H
