#include "object.h"

// ----------------------
// Base Object class methods
// ----------------------
Object::Object(float mass, const Vector2D& position, const Vector2D& velocity)
    : mass(mass), position(position), velocity(velocity), acceleration(0, 0), netForce(0, 0) {}

float Object::getMass() const {
    return mass;
}

void Object::setMass(float mass) {
    this->mass = mass;
}

const Vector2D& Object::getPosition() const {
    return position;
}

void Object::setPosition(const Vector2D& position) {
    this->position = position;
}

const Vector2D& Object::getVelocity() const {
    return velocity;
}

void Object::setVelocity(const Vector2D& velocity) {
    this->velocity = velocity;
}

const Vector2D& Object::getAcceleration() const {
    return acceleration;
}

void Object::applyForce(const Vector2D& force) {
    netForce += force;
}

void Object::update(float deltaTime) {
    if (mass != 0) {
        acceleration = calculateAcceleration();
    }
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    resetForces();
}

void Object::resetForces() {
    netForce = Vector2D(0, 0);
}

Vector2D Object::calculateAcceleration() const {
    return netForce / mass;
}


// ----------------------
// Square class methods
// ----------------------
Square::Square(float mass, const Vector2D& position, const Vector2D& velocity, float sideLength)
    : Object(mass, position, velocity), sideLength(sideLength) {}

float Square::getSideLength() const {
    return sideLength;
}

void Square::setSideLength(float sideLength) {
    this->sideLength = sideLength;
}

float Square::calculateArea() const {
    return sideLength * sideLength;
}


// ----------------------
// Rectangle class methods
// ----------------------
Rectangle::Rectangle(float mass, const Vector2D& position, const Vector2D& velocity, float width, float height)
    : Object(mass, position, velocity), width(width), height(height) {}

float Rectangle::getWidth() const {
    return width;
}

float Rectangle::getHeight() const {
    return height;
}

void Rectangle::setDimensions(float width, float height) {
    this->width = width;
    this->height = height;
}

float Rectangle::calculateArea() const {
    return width * height;
}


// ----------------------
// Circle class methods
// ----------------------
Circle::Circle(float mass, const Vector2D& position, const Vector2D& velocity, float radius)
    : Object(mass, position, velocity), radius(radius) {}

float Circle::getRadius() const {
    return radius;
}

void Circle::setRadius(float radius) {
    this->radius = radius;
}

float Circle::calculateArea() const {
    return 3.14159f * radius * radius;  // Area of a circle = πr²
}

// ----------------------
// CustomShape class methods
// ----------------------
CustomShape::CustomShape(float mass, const Vector2D& position, const Vector2D& velocity, const std::vector<Vector2D>& vertices)
    : Object(mass, position, velocity), vertices(vertices) {}

const std::vector<Vector2D>& CustomShape::getVertices() const {
    return vertices;
}

void CustomShape::setVertices(const std::vector<Vector2D>& vertices) {
    this->vertices = vertices;
}

float CustomShape::calculateArea() const {
    return calculatePolygonArea();
}


// Helper function to calculate the area of a polygon using the shoelace formula
float CustomShape::calculatePolygonArea() const {
    float area = 0;
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        const Vector2D& p1 = vertices[i];
        const Vector2D& p2 = vertices[(i + 1) % n];
        area += (p1.x * p2.y) - (p1.y * p2.x);
    }
    return std::abs(area) / 2.0f;
}
