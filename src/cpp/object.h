#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "vector2d.h"
#include "collider.h"

// Base Object class
class Object {
public:
    // Constructor with ID, mass, position, and velocity
    Object(int id, float mass, const Vector2D& position, const Vector2D& velocity);

    virtual ~Object() {
        delete collider;  // Ensure proper destruction of the collider
    }

    // Getters and setters for object properties
    float getMass() const;
    void setMass(float mass);

    const Vector2D& getPosition() const;
    void setPosition(const Vector2D& position);
    
    const Vector2D& getVelocity() const;
    void setVelocity(const Vector2D& velocity);

    const Vector2D& getAcceleration() const;

    // Modifiable getters for object properties
    Vector2D& getPosition() { return position; }
    Vector2D& getVelocity() { return velocity; }
    Vector2D& getAcceleration() { return acceleration; }

    // Apply force to the object
    void applyForce(const Vector2D& force);

    // Update object's state (virtual to allow overriding in derived classes)
    virtual void update(float deltaTime);

    // Reset accumulated forces
    void resetForces();

    // Method to calculate area (pure virtual, implemented by derived classes)
    virtual float calculateArea() const = 0;

    // New method to get the object's unique ID
    int getID() const;

    // New method to update position based on velocity and deltaTime
    void updatePosition(float deltaTime);

    virtual Collider* getCollider() const = 0;  // Must be implemented by derived classes

protected:
    int id;              
    float mass;
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    Vector2D netForce;

    Collider* collider;  // Pointer to the collider

    // Helper function to calculate acceleration
    Vector2D calculateAcceleration() const;
};

// Derived class: Square
class Square : public Object {
public:
    Square(int id, float mass, const Vector2D& position, const Vector2D& velocity, float sideLength)
        : Object(id, mass, position, velocity), sideLength(sideLength) {
        collider = new AABBCollider(position, sideLength, sideLength);  // Create an AABB collider for the square
    }

    float getSideLength() const;
    void setSideLength(float sideLength);

    // Override the calculateArea method
    float calculateArea() const override;

    // Override getCollider to return the collider
    Collider* getCollider() const override {
        return collider;
    }

private:
    float sideLength;
};

// Derived class: Rectangle
class Rectangle : public Object {
public:
    Rectangle(int id, float mass, const Vector2D& position, const Vector2D& velocity, float width, float height)
        : Object(id, mass, position, velocity), width(width), height(height) {
        collider = new AABBCollider(position, width, height);  // Create an AABB collider for the rectangle
    }

    float getWidth() const;
    float getHeight() const;
    void setDimensions(float width, float height);

    // Override the calculateArea method
    float calculateArea() const override;

    // Override getCollider to return the collider
    Collider* getCollider() const override {
        return collider;
    }

private:
    float width, height;
};


// Derived class: Circle
class Circle : public Object {
public:
    Circle(int id, float mass, const Vector2D& position, const Vector2D& velocity, float radius)
        : Object(id, mass, position, velocity), radius(radius) {
        collider = new CircleCollider(position, radius);  // Create a Circle collider for the circle
    }

    float getRadius() const;
    void setRadius(float radius);

    // Override the calculateArea method
    float calculateArea() const override;

    // Override getCollider to return the collider
    Collider* getCollider() const override {
        return collider;
    }

private:
    float radius;
};

// Derived class: CustomShape (polygon)
class CustomShape : public Object {
public:
    CustomShape(int id, float mass, const Vector2D& position, const Vector2D& velocity, const std::vector<Vector2D>& vertices)
        : Object(id, mass, position, velocity), vertices(vertices) {
        // Create an AABB collider based on the bounding box of the polygon
        collider = new AABBCollider(position, calculateBoundingBoxWidth(), calculateBoundingBoxHeight());
    }

    const std::vector<Vector2D>& getVertices() const { return vertices; }
    void setVertices(const std::vector<Vector2D>& vertices) {
        this->vertices = vertices;
        // Update the collider when vertices change
        collider = new AABBCollider(position, calculateBoundingBoxWidth(), calculateBoundingBoxHeight());
    }

    // Override the calculateArea method
    float calculateArea() const override {
        return calculatePolygonArea();
    }

    // Override getCollider to return the collider
    Collider* getCollider() const override {
        return collider;
    }

private:
    std::vector<Vector2D> vertices;

    // Helper function to calculate the area of the polygon
    float calculatePolygonArea() const {
        float area = 0.0f;
        int n = vertices.size();
        
        // Ensure we have at least 3 vertices to form a valid polygon
        if (n < 3) return 0.0f;

        for (int i = 0; i < n; i++) {
            const Vector2D& current = vertices[i];
            const Vector2D& next = vertices[(i + 1) % n];
            area += (current.x * next.y) - (next.x * current.y);
        }
        return 0.5f * std::abs(area);
    }

    // Helper functions to calculate the bounding box dimensions for the AABB collider
    float calculateBoundingBoxWidth() const {
        float minX = vertices[0].x, maxX = vertices[0].x;
        for (const Vector2D& v : vertices) {
            if (v.x < minX) minX = v.x;
            if (v.x > maxX) maxX = v.x;
        }
        return maxX - minX;
    }

    float calculateBoundingBoxHeight() const {
        float minY = vertices[0].y, maxY = vertices[0].y;
        for (const Vector2D& v : vertices) {
            if (v.y < minY) minY = v.y;
            if (v.y > maxY) maxY = v.y;
        }
        return maxY - minY;
    }
};

#endif // OBJECT_H
