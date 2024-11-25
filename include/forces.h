#ifndef FORCES_H
#define FORCES_H

#include "vector2d.h"
#include "object.h"

// Define the gravitational constant (default value for Earth's gravity)
const float DEFAULT_GRAVITY = 9.8f;

class Forces {
public:
    Forces();

    // Method to apply user-defined custom force
    static void applyCustomForce(Object& object, const Vector2D& force);

    // Method to apply gravity force (global, affecting all objects)
    static void applyGravity(Object& object, float gravity = DEFAULT_GRAVITY);

    // Methods for friction simulation
    // Static friction: prevents motion until force threshold is overcome
    static void applyStaticFriction(Object& object, float staticFrictionCoefficient, const Vector2D& surfaceNormal);

    // Kinetic friction: opposes motion, proportional to velocity
    static void applyKineticFriction(Object& object, float kineticFrictionCoefficient, const Vector2D& surfaceNormal);

    // Toggle gravity on/off
    static void toggleGravity(bool enable);

private:
    static bool gravityEnabled;  // Internal state to check if gravity is active
};

#endif // FORCES_H
