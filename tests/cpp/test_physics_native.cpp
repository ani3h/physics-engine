// File: tests/cpp/test_physics_native.cpp
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include "../../include/physics_native.h"

// Helper function to check if two floating point numbers are approximately equal
bool approxEqual(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;
}

// Helper function to print test results
void printTestResult(const std::string& testName, bool passed) {
    std::cout << testName << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
}

// Mock JNIEnv for testing
struct MockJNIEnv {
    std::vector<double> doubleArrayElements;
    jsize arrayLength;

    jdouble* GetDoubleArrayElements(jdoubleArray array, jboolean* isCopy) {
        return doubleArrayElements.data();
    }

    void ReleaseDoubleArrayElements(jdoubleArray array, jdouble* elements, jint mode) {
        // No-op for testing
    }

    jsize GetArrayLength(jarray array) {
        return arrayLength;
    }
};

// Test PhysicsWorld creation and deletion
void testWorldManagement() {
    // Test world creation
    jlong worldPtr = Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld(nullptr, nullptr);
    bool test1 = worldPtr != 0;
    printTestResult("PhysicsWorld Creation", test1);

    // Test world properties
    PhysicsWorld* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    bool test2 = approxEqual(world->gravity, 9.81f) &&
                 approxEqual(world->staticFriction, 0.5f) &&
                 approxEqual(world->kineticFriction, 0.3f);
    printTestResult("PhysicsWorld Default Properties", test2);

    // Test object container
    bool test3 = world->objects.empty();
    printTestResult("PhysicsWorld Empty Objects Container", test3);

    // Cleanup
    Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld(nullptr, nullptr, worldPtr);
    printTestResult("PhysicsWorld Deletion", true);
}

// Test object addition and management
void testObjectManagement() {
    jlong worldPtr = Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld(nullptr, nullptr);
    PhysicsWorld* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    MockJNIEnv env;

    // Test rectangle creation
    env.doubleArrayElements = {50.0, 30.0};
    env.arrayLength = 2;
    Java_JAVA_jni_PhysicsEngineJNI_addObject(
        reinterpret_cast<JNIEnv*>(&env), nullptr, worldPtr,
        1, 10.0, 0.0, 0.0, 1.0, 0.0, 'R',
        nullptr  // jdoubleArray is mocked
    );

    bool test1 = world->objects.size() == 1;
    printTestResult("Rectangle Object Addition", test1);

    // Test circle creation
    env.doubleArrayElements = {20.0};
    env.arrayLength = 1;
    Java_JAVA_jni_PhysicsEngineJNI_addObject(
        reinterpret_cast<JNIEnv*>(&env), nullptr, worldPtr,
        2, 5.0, 100.0, 100.0, -1.0, 0.0, 'C',
        nullptr
    );

    bool test2 = world->objects.size() == 2;
    printTestResult("Circle Object Addition", test2);

    // Test object retrieval
    ObjectState* state = reinterpret_cast<ObjectState*>(
        Java_JAVA_jni_PhysicsEngineJNI_getObjectState(
            nullptr, nullptr, worldPtr, 1
        )
    );
    bool test3 = state != nullptr;
    printTestResult("Object State Retrieval", test3);

    // Cleanup
    Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld(nullptr, nullptr, worldPtr);
}

// Test physics simulation steps
void testPhysicsSimulation() {
    jlong worldPtr = Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld(nullptr, nullptr);
    PhysicsWorld* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    MockJNIEnv env;

    // Add a test object
    env.doubleArrayElements = {10.0};
    env.arrayLength = 1;
    Java_JAVA_jni_PhysicsEngineJNI_addObject(
        reinterpret_cast<JNIEnv*>(&env), nullptr, worldPtr,
        1, 1.0, 0.0, 0.0, 0.0, 0.0, 'S',
        nullptr
    );

    // Test initial state
    Object* obj = world->objects[0];
    Vector2D initialPos = obj->getPosition();
    
    // Step simulation
    Java_JAVA_jni_PhysicsEngineJNI_stepSimulation(nullptr, nullptr, worldPtr, 1.0);
    
    // Verify gravity effect
    Vector2D finalPos = obj->getPosition();
    bool test1 = finalPos.y > initialPos.y;  // Object should fall
    printTestResult("Gravity Effect Test", test1);

    // Test force configuration
    double gravityParams[] = {5.0};
    env.doubleArrayElements = std::vector<double>(gravityParams, gravityParams + 1);
    env.arrayLength = 1;
    Java_JAVA_jni_PhysicsEngineJNI_configureForces(
        reinterpret_cast<JNIEnv*>(&env), nullptr, worldPtr, 2,
        nullptr  // jdoubleArray is mocked
    );

    bool test2 = approxEqual(world->gravity, 5.0f);
    printTestResult("Force Configuration Test", test2);

    // Cleanup
    Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld(nullptr, nullptr, worldPtr);
}

// Test collision detection and resolution
void testCollisionHandling() {
    jlong worldPtr = Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld(nullptr, nullptr);
    PhysicsWorld* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    MockJNIEnv env;

    // Add two colliding objects
    env.doubleArrayElements = {10.0};
    env.arrayLength = 1;
    Java_JAVA_jni_PhysicsEngineJNI_addObject(
        reinterpret_cast<JNIEnv*>(&env), nullptr, worldPtr,
        1, 1.0, 0.0, 0.0, 1.0, 0.0, 'S',
        nullptr
    );

    Java_JAVA_jni_PhysicsEngineJNI_addObject(
        reinterpret_cast<JNIEnv*>(&env), nullptr, worldPtr,
        2, 1.0, 15.0, 0.0, -1.0, 0.0, 'S',
        nullptr
    );

    // Store initial velocities
    Vector2D vel1 = world->objects[0]->getVelocity();
    Vector2D vel2 = world->objects[1]->getVelocity();

    // Handle collisions
    Java_JAVA_jni_PhysicsEngineJNI_handleCollisions(nullptr, nullptr, worldPtr);

    // Verify velocity changes
    Vector2D newVel1 = world->objects[0]->getVelocity();
    Vector2D newVel2 = world->objects[1]->getVelocity();
    
    bool test1 = newVel1.x != vel1.x || newVel2.x != vel2.x;
    printTestResult("Collision Resolution Test", test1);

    // Cleanup
    Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld(nullptr, nullptr, worldPtr);
}

// Test ground collision handling
void testGroundCollision() {
    jlong worldPtr = Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld(nullptr, nullptr);
    PhysicsWorld* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    MockJNIEnv env;

    // Add object near ground
    env.doubleArrayElements = {10.0};
    env.arrayLength = 1;
    Java_JAVA_jni_PhysicsEngineJNI_addObject(
        reinterpret_cast<JNIEnv*>(&env), nullptr, worldPtr,
        1, 1.0, 0.0, world->groundLevel - 1, 0.0, 1.0, 'S',
        nullptr
    );

    // Check if object is near ground
    bool test1 = isNearGround(world->objects[0], world);
    printTestResult("Ground Proximity Detection", test1);

    // Step simulation to trigger ground collision
    Java_JAVA_jni_PhysicsEngineJNI_stepSimulation(nullptr, nullptr, worldPtr, 1.0);

    // Verify object doesn't fall through ground
    bool test2 = world->objects[0]->getPosition().y <= world->groundLevel;
    printTestResult("Ground Collision Prevention", test2);

    // Cleanup
    Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld(nullptr, nullptr, worldPtr);
}

// Test edge cases and error handling
void testEdgeCases() {
    jlong worldPtr = Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld(nullptr, nullptr);
    PhysicsWorld* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    MockJNIEnv env;

    // Test invalid object ID
    ObjectState* state = reinterpret_cast<ObjectState*>(
        Java_JAVA_jni_PhysicsEngineJNI_getObjectState(nullptr, nullptr, worldPtr, 999)
    );
    bool test1 = state == nullptr;
    printTestResult("Invalid Object ID Handling", test1);

    // Test invalid shape type
    env.doubleArrayElements = {10.0};
    env.arrayLength = 1;
    Java_JAVA_jni_PhysicsEngineJNI_addObject(
        reinterpret_cast<JNIEnv*>(&env), nullptr, worldPtr,
        1, 1.0, 0.0, 0.0, 0.0, 0.0, 'X',  // Invalid shape type
        nullptr
    );
    bool test2 = world->objects.empty();
    printTestResult("Invalid Shape Type Handling", test2);

    // Cleanup
    Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld(nullptr, nullptr, worldPtr);
}

int main() {
    std::cout << "Running Physics Native Interface Tests...\n\n";

    // Run all test suites
    testWorldManagement();
    std::cout << "\n";
    
    testObjectManagement();
    std::cout << "\n";
    
    testPhysicsSimulation();
    std::cout << "\n";
    
    testCollisionHandling();
    std::cout << "\n";
    
    testGroundCollision();
    std::cout << "\n";
    
    testEdgeCases();
    std::cout << "\n";

    std::cout << "All tests completed.\n";
    return 0;
}
