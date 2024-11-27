// File: tests/cpp/test_object.cpp
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include "../../include/object.h"
#include "../../include/vector2d.h"

// Helper function to check if two floating point numbers are approximately equal
bool approxEqual(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;
}

// Helper function to print test results
void printTestResult(const std::string& testName, bool passed) {
    std::cout << testName << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
}

// Test Square class
void testSquare() {
    // Test creation and basic properties
    Square square(1, 10.0f, Vector2D(0, 0), Vector2D(1, 1), 5.0f);
    
    bool test1 = square.getID() == 1 &&
                 approxEqual(square.getMass(), 10.0f) &&
                 approxEqual(square.getSideLength(), 5.0f);
    printTestResult("Square Creation Test", test1);

    // Test area calculation
    bool test2 = approxEqual(square.calculateArea(), 25.0f); // 5 * 5
    printTestResult("Square Area Calculation", test2);

    // Test collider creation
    bool test3 = square.getCollider() != nullptr;
    printTestResult("Square Collider Creation", test3);

    // Test property modifications
    square.setMass(20.0f);
    square.setSideLength(10.0f);
    square.setPosition(Vector2D(5, 5));
    square.setVelocity(Vector2D(2, 2));

    bool test4 = approxEqual(square.getMass(), 20.0f) &&
                 approxEqual(square.getSideLength(), 10.0f) &&
                 approxEqual(square.getPosition().x, 5.0f) &&
                 approxEqual(square.getPosition().y, 5.0f);
    printTestResult("Square Property Modification", test4);
}

// Test Rectangle class
void testRectangle() {
    // Test creation and basic properties
    Rectangle rect(2, 15.0f, Vector2D(1, 1), Vector2D(1, 1), 6.0f, 4.0f);
    
    bool test1 = rect.getID() == 2 &&
                 approxEqual(rect.getMass(), 15.0f) &&
                 approxEqual(rect.getWidth(), 6.0f) &&
                 approxEqual(rect.getHeight(), 4.0f);
    printTestResult("Rectangle Creation Test", test1);

    // Test area calculation
    bool test2 = approxEqual(rect.calculateArea(), 24.0f); // 6 * 4
    printTestResult("Rectangle Area Calculation", test2);

    // Test collider creation
    bool test3 = rect.getCollider() != nullptr;
    printTestResult("Rectangle Collider Creation", test3);

    // Test property modifications
    rect.setMass(25.0f);
    rect.setDimensions(8.0f, 5.0f);
    rect.setPosition(Vector2D(10, 10));

    bool test4 = approxEqual(rect.getMass(), 25.0f) &&
                 approxEqual(rect.getWidth(), 8.0f) &&
                 approxEqual(rect.getHeight(), 5.0f);
    printTestResult("Rectangle Property Modification", test4);
}

// Test Circle class
void testCircle() {
    // Test creation and basic properties
    Circle circle(3, 12.0f, Vector2D(2, 2), Vector2D(1, 1), 3.0f);
    
    bool test1 = circle.getID() == 3 &&
                 approxEqual(circle.getMass(), 12.0f) &&
                 approxEqual(circle.getRadius(), 3.0f);
    printTestResult("Circle Creation Test", test1);

    // Test area calculation
    bool test2 = approxEqual(circle.calculateArea(), 28.27433f); // π * 3²
    printTestResult("Circle Area Calculation", test2);

    // Test collider creation
    bool test3 = circle.getCollider() != nullptr;
    printTestResult("Circle Collider Creation", test3);

    // Test property modifications
    circle.setMass(30.0f);
    circle.setRadius(5.0f);
    circle.setPosition(Vector2D(15, 15));

    bool test4 = approxEqual(circle.getMass(), 30.0f) &&
                 approxEqual(circle.getRadius(), 5.0f);
    printTestResult("Circle Property Modification", test4);
}

// Test CustomShape class
void testCustomShape() {
    // Create a triangle shape
    std::vector<Vector2D> vertices = {
        Vector2D(0, 0),
        Vector2D(3, 0),
        Vector2D(0, 4)
    };
    
    CustomShape shape(4, 20.0f, Vector2D(0, 0), Vector2D(1, 1), vertices);
    
    // Test creation and basic properties
    bool test1 = shape.getID() == 4 &&
                 approxEqual(shape.getMass(), 20.0f) &&
                 shape.getVertices().size() == 3;
    printTestResult("CustomShape Creation Test", test1);

    // Test area calculation (triangle area = 6)
    bool test2 = approxEqual(shape.calculateArea(), 6.0f);
    printTestResult("CustomShape Area Calculation", test2);

    // Test collider creation
    bool test3 = shape.getCollider() != nullptr;
    printTestResult("CustomShape Collider Creation", test3);

    // Test vertex modification
    std::vector<Vector2D> newVertices = {
        Vector2D(0, 0),
        Vector2D(4, 0),
        Vector2D(0, 3)
    };
    shape.setVertices(newVertices);
    
    bool test4 = shape.getVertices().size() == 3;
    printTestResult("CustomShape Vertex Modification", test4);
}

// Test physics behavior
void testPhysicsBehavior() {
    // Test force application and movement
    Square testObj(5, 2.0f, Vector2D(0, 0), Vector2D(0, 0), 1.0f);
    
    // Apply force and update
    Vector2D force(10.0f, 5.0f);
    testObj.applyForce(force);
    testObj.update(1.0f);  // Update with 1 second time step

    // Check acceleration (F = ma, so a = F/m)
    Vector2D expectedAcc = force / 2.0f;
    bool test1 = approxEqual(testObj.getAcceleration().x, expectedAcc.x) &&
                 approxEqual(testObj.getAcceleration().y, expectedAcc.y);
    printTestResult("Force Application Test", test1);

    // Test velocity update (v = v0 + at)
    Vector2D expectedVel = Vector2D(5.0f, 2.5f);  // acceleration * time
    bool test2 = approxEqual(testObj.getVelocity().x, expectedVel.x) &&
                 approxEqual(testObj.getVelocity().y, expectedVel.y);
    printTestResult("Velocity Update Test", test2);

    // Test position update (x = x0 + vt + 1/2at²)
    Vector2D expectedPos = Vector2D(5.0f, 2.5f);  // v*t + 0.5*a*t^2
    bool test3 = approxEqual(testObj.getPosition().x, expectedPos.x) &&
                 approxEqual(testObj.getPosition().y, expectedPos.y);
    printTestResult("Position Update Test", test3);

    // Test force reset
    testObj.resetForces();
    testObj.update(1.0f);
    bool test4 = approxEqual(testObj.getAcceleration().x, 0.0f) &&
                 approxEqual(testObj.getAcceleration().y, 0.0f);
    printTestResult("Force Reset Test", test4);
}

// Test edge cases
void testEdgeCases() {
    // Test zero mass
    Square zeroMassObj(6, 0.0f, Vector2D(0, 0), Vector2D(0, 0), 1.0f);
    zeroMassObj.applyForce(Vector2D(10, 10));
    zeroMassObj.update(1.0f);
    
    bool test1 = approxEqual(zeroMassObj.getAcceleration().x, 0.0f) &&
                 approxEqual(zeroMassObj.getAcceleration().y, 0.0f);
    printTestResult("Zero Mass Handling", test1);

    // Test zero size
    Square zeroSizeObj(7, 1.0f, Vector2D(0, 0), Vector2D(0, 0), 0.0f);
    bool test2 = approxEqual(zeroSizeObj.calculateArea(), 0.0f);
    printTestResult("Zero Size Handling", test2);

    // Test negative values
    Circle negativeRadius(8, 1.0f, Vector2D(0, 0), Vector2D(0, 0), -1.0f);
    bool test3 = approxEqual(negativeRadius.calculateArea(), 3.14159f); // Should use absolute value
    printTestResult("Negative Size Handling", test3);
}

int main() {
    std::cout << "Running Object Class Tests...\n\n";

    // Run all test suites
    testSquare();
    std::cout << "\n";
    
    testRectangle();
    std::cout << "\n";
    
    testCircle();
    std::cout << "\n";
    
    testCustomShape();
    std::cout << "\n";
    
    testPhysicsBehavior();
    std::cout << "\n";
    
    testEdgeCases();
    std::cout << "\n";

    std::cout << "All tests completed.\n";
    return 0;
}
