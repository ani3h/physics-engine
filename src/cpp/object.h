#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "vector2d.h"

// Base Object class
class Object {
public:
    Object(float mass, const Vector2D& position, const Vector2D& velocity);

    virtual ~Object() = default;  // Ensure proper destruction of derived objects

    // Getters and setters for object properties
    float getMass() const;
    void setMass(float mass);

    const Vector2D& getPosition() const;
    void setPosition(const Vector2D& position);
    
    const Vector2D& getVelocity() const;
    void setVelocity(const Vector2D& velocity);

    const Vector2D& getAcceleration() const;

    // Modifiable getters for object properties
    Vector2D& getPosition() {return position;}
    Vector2D& getVelocity() {return velocity;}
    Vector2D& getAcceleration() {return acceleration;}

    // Apply force to the object
    void applyForce(const Vector2D& force);

    // Update object's state (virtual to allow overriding in derived classes)
    virtual void update(float deltaTime);

    // Reset accumulated forces
    void resetForces();

    // Method to calculate area (pure virtual, implemented by derived classes)
    virtual float calculateArea() const = 0;

protected:
    float mass;
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    Vector2D netForce;

    // Helper function to calculate acceleration
    Vector2D calculateAcceleration() const;
};

// Derived class: Square
class Square : public Object {
public:
    Square(float mass, const Vector2D& position, const Vector2D& velocity, float sideLength);

    float getSideLength() const;
    void setSideLength(float sideLength);

    // Override the calculateArea method
    float calculateArea() const override;

private:
    float sideLength;
};

// Derived class: Rectangle
class Rectangle : public Object {
public:
    Rectangle(float mass, const Vector2D& position, const Vector2D& velocity, float width, float height);

    float getWidth() const;
    float getHeight() const;
    void setDimensions(float width, float height);

    // Override the calculateArea method
    float calculateArea() const override;

private:
    float width, height;
};

// Derived class: Circle
class Circle : public Object {
public:
    Circle(float mass, const Vector2D& position, const Vector2D& velocity, float radius);

    float getRadius() const;
    void setRadius(float radius);

    // Override the calculateArea method
    float calculateArea() const override;

private:
    float radius;
};

// Derived class: CustomShape (polygon)
class CustomShape : public Object {
public:
    CustomShape(float mass, const Vector2D& position, const Vector2D& velocity, const std::vector<Vector2D>& vertices);

    const std::vector<Vector2D>& getVertices() const;
    void setVertices(const std::vector<Vector2D>& vertices);

    // Override the calculateArea method
    float calculateArea() const override;

private:
    std::vector<Vector2D> vertices;

    // Helper function to calculate the area of a polygon
    float calculatePolygonArea() const;
};

#endif // OBJECT_H
