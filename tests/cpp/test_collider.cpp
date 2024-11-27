
// File: tests/cpp/test_colliders.cpp
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include "../../include/collider.h"
#include "../../include/object.h"

// Helper function to print test results
void printTestResult(const std::string& testName, bool passed) {
    std::cout << testName << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
}

// Test AABB vs AABB collision
void testAABBCollision() {
    // Test 1: Overlapping boxes
    AABBCollider box1(Vector2D(0, 0), 10, 10);
    AABBCollider box2(Vector2D(5, 5), 10, 10);
    bool result1 = box1.checkCollision(box2);
    printTestResult("AABB Overlap Test", result1);

    // Test 2: Non-overlapping boxes
    AABBCollider box3(Vector2D(0, 0), 10, 10);
    AABBCollider box4(Vector2D(20, 20), 10, 10);
    bool result2 = box3.checkCollision(box4);
    printTestResult("AABB No Overlap Test", !result2);

    // Test 3: Touching boxes (edge contact)
    AABBCollider box5(Vector2D(0, 0), 10, 10);
    AABBCollider box6(Vector2D(10, 0), 10, 10);
    bool result3 = box5.checkCollision(box6);
    printTestResult("AABB Edge Contact Test", result3);

    // Test 4: Touching boxes (corner contact)
    AABBCollider box7(Vector2D(0, 0), 10, 10);
    AABBCollider box8(Vector2D(10, 10), 10, 10);
    bool result4 = box7.checkCollision(box8);
    printTestResult("AABB Corner Contact Test", result4);
}

// Test Circle vs Circle collision
void testCircleCollision() {
    // Test 1: Overlapping circles
    CircleCollider circle1(Vector2D(0, 0), 5);
    CircleCollider circle2(Vector2D(8, 0), 5);
    bool result1 = circle1.checkCollision(circle2);
    printTestResult("Circle Overlap Test", result1);

    // Test 2: Non-overlapping circles
    CircleCollider circle3(Vector2D(0, 0), 5);
    CircleCollider circle4(Vector2D(15, 0), 5);
    bool result2 = circle3.checkCollision(circle4);
    printTestResult("Circle No Overlap Test", !result2);

    // Test 3: Touching circles
    CircleCollider circle5(Vector2D(0, 0), 5);
    CircleCollider circle6(Vector2D(10, 0), 5);
    bool result3 = circle5.checkCollision(circle6);
    printTestResult("Circle Touch Test", result3);

    // Test 4: Concentric circles
    CircleCollider circle7(Vector2D(0, 0), 5);
    CircleCollider circle8(Vector2D(0, 0), 3);
    bool result4 = circle7.checkCollision(circle8);
    printTestResult("Circle Concentric Test", result4);
}

// Test AABB vs Circle collision
void testAABBCircleCollision() {
    // Test 1: Circle overlapping box
    AABBCollider box1(Vector2D(0, 0), 10, 10);
    CircleCollider circle1(Vector2D(8, 8), 5);
    bool result1 = box1.checkCollision(circle1);
    printTestResult("Box-Circle Overlap Test", result1);

    // Test 2: Circle not overlapping box
    AABBCollider box2(Vector2D(0, 0), 10, 10);
    CircleCollider circle2(Vector2D(20, 20), 5);
    bool result2 = box2.checkCollision(circle2);
    printTestResult("Box-Circle No Overlap Test", !result2);

    // Test 3: Circle touching box edge
    AABBCollider box3(Vector2D(0, 0), 10, 10);
    CircleCollider circle3(Vector2D(15, 5), 5);
    bool result3 = box3.checkCollision(circle3);
    printTestResult("Box-Circle Edge Touch Test", result3);

    // Test 4: Circle touching box corner
    AABBCollider box4(Vector2D(0, 0), 10, 10);
    CircleCollider circle4(Vector2D(15, 15), 7.07107f); // sqrt(50)
    bool result4 = box4.checkCollision(circle4);
    printTestResult("Box-Circle Corner Touch Test", result4);
}

// Test collision resolution
void testCollisionResolution() {
    // Test AABB collision resolution
    Rectangle rect1(1, 1.0f, Vector2D(0, 0), Vector2D(1, 0), 10, 10);
    Rectangle rect2(2, 1.0f, Vector2D(8, 0), Vector2D(-1, 0), 10, 10);
    
    AABBCollider* collider = static_cast<AABBCollider*>(rect1.getCollider());
    collider->resolveCollision(rect1, rect2);
    
    // Verify velocities have changed
    bool velocitiesChanged = (rect1.getVelocity().x != 1 || rect2.getVelocity().x != -1);
    printTestResult("AABB Collision Resolution Test", velocitiesChanged);

    // Test Circle collision resolution
    Circle circle1(3, 1.0f, Vector2D(0, 0), Vector2D(1, 0), 5);
    Circle circle2(4, 1.0f, Vector2D(8, 0), Vector2D(-1, 0), 5);
    
    CircleCollider* circleCollider = static_cast<CircleCollider*>(circle1.getCollider());
    circleCollider->resolveCollision(circle1, circle2);
    
    velocitiesChanged = (circle1.getVelocity().x != 1 || circle2.getVelocity().x != -1);
    printTestResult("Circle Collision Resolution Test", velocitiesChanged);
}

// Test edge cases
void testEdgeCases() {
    // Test 1: Zero-size AABB
    AABBCollider box1(Vector2D(0, 0), 0, 0);
    AABBCollider box2(Vector2D(0, 0), 10, 10);
    bool result1 = box1.checkCollision(box2);
    printTestResult("Zero-size AABB Test", !result1);

    // Test 2: Zero-radius Circle
    CircleCollider circle1(Vector2D(0, 0), 0);
    CircleCollider circle2(Vector2D(0, 0), 5);
    bool result2 = circle1.checkCollision(circle2);
    printTestResult("Zero-radius Circle Test", result2);

    // Test 3: Negative dimensions (should be handled gracefully)
    AABBCollider box3(Vector2D(0, 0), -10, -10);
    AABBCollider box4(Vector2D(5, 5), 10, 10);
    bool result3 = box3.checkCollision(box4);
    printTestResult("Negative Dimensions Test", !result3);
}

int main() {
    std::cout << "Running Collision Detection Tests...\n\n";

    // Run all test suites
    testAABBCollision();
    std::cout << "\n";
    
    testCircleCollision();
    std::cout << "\n";
    
    testAABBCircleCollision();
    std::cout << "\n";
    
    testCollisionResolution();
    std::cout << "\n";
    
    testEdgeCases();
    std::cout << "\n";

    std::cout << "All tests completed.\n";
    return 0;
}