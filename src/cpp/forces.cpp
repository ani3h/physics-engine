#include "forces.h"

// Static member initialization
bool Forces::gravityEnabled = true;

// Constructor (can be expanded if needed)
Forces::Forces() {}


// -------------------
// Custom Force Method
// -------------------
void Forces::applyCustomForce(Object& object, const Vector2D& force) {
    object.applyForce(force);
}


// -------------------
// Gravity Simulation Method
// -------------------
void Forces::applyGravity(Object& object, float gravity) {
    if (gravityEnabled && object.getMass() > 0) {
        Vector2D gravityForce(0, gravity * object.getMass());  // Apply downward force
        object.applyForce(gravityForce);
    }
}

void Forces::toggleGravity(bool enable) {
    gravityEnabled = enable;
}


// -------------------
// Static Friction Simulation
// -------------------
void Forces::applyStaticFriction(Object& object, float staticFrictionCoefficient, const Vector2D& surfaceNormal) {
    // No friction applied if object is moving
    if (object.getVelocity().length() > 0) {
        return;
    }

    // Calculate static friction force magnitude (force normal * static coefficient)
    Vector2D frictionForce = surfaceNormal * (staticFrictionCoefficient * object.getMass());

    // Apply the static friction force to object, preventing motion
    object.applyForce(-frictionForce);  // Opposes any applied forces
}


// -------------------
// Kinetic Friction Simulation
// -------------------
void Forces::applyKineticFriction(Object& object, float kineticFrictionCoefficient, const Vector2D& surfaceNormal) {
    if (object.getVelocity().length() == 0) {
        return;
    }

    // Friction force is proportional to the velocity and opposite to the motion direction
    Vector2D frictionDirection = object.getVelocity().normalized() * -1;  // Opposite to velocity
    Vector2D frictionForce = frictionDirection * (kineticFrictionCoefficient * object.getMass());

    // Apply kinetic friction force to the object
    object.applyForce(frictionForce);
}
