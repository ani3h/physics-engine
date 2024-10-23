#include "object.h"

// Constructor with ID, mass, position, and velocity
Object::Object(int id, float mass, const Vector2D& position, const Vector2D& velocity)
    : id(id), mass(mass), position(position), velocity(velocity), acceleration(0, 0), netForce(0, 0) {}

// Get the object's unique ID
int Object::getID() const {
    return id;
}

// Update position based on velocity and deltaTime
void Object::updatePosition(float deltaTime) {
    position += velocity * deltaTime;
}

// Getters and setters
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

// Apply force to the object
void Object::applyForce(const Vector2D& force) {
    netForce += force;
}

// Update object's state
void Object::update(float deltaTime) {
    acceleration = calculateAcceleration();
    velocity += acceleration * deltaTime;
    updatePosition(deltaTime);  // Move the object
}

// Reset accumulated forces
void Object::resetForces() {
    netForce = Vector2D(0, 0);
}

// Helper function to calculate acceleration
Vector2D Object::calculateAcceleration() const {
    return mass > 0 ? netForce / mass : Vector2D(0, 0);
}

// Constructor for Square
Square::Square(int id, float mass, const Vector2D& position, const Vector2D& velocity, float sideLength)
    : Object(id, mass, position, velocity), sideLength(sideLength) {}

float Square::getSideLength() const {
    return sideLength;
}

void Square::setSideLength(float sideLength) {
    this->sideLength = sideLength;
}

float Square::calculateArea() const {
    return sideLength * sideLength;
}

// Constructor for Rectangle
Rectangle::Rectangle(int id, float mass, const Vector2D& position, const Vector2D& velocity, float width, float height)
    : Object(id, mass, position, velocity), width(width), height(height) {}

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

// Constructor for Circle
Circle::Circle(int id, float mass, const Vector2D& position, const Vector2D& velocity, float radius)
    : Object(id, mass, position, velocity), radius(radius) {}

float Circle::getRadius() const {
    return radius;
}

void Circle::setRadius(float radius) {
    this->radius = radius;
}

float Circle::calculateArea() const {
    return 3.14159f * radius * radius;
}

// Constructor for CustomShape
CustomShape::CustomShape(int id, float mass, const Vector2D& position, const Vector2D& velocity, const std::vector<Vector2D>& vertices)
    : Object(id, mass, position, velocity), vertices(vertices) {}

const std::vector<Vector2D>& CustomShape::getVertices() const {
    return vertices;
}

void CustomShape::setVertices(const std::vector<Vector2D>& vertices) {
    this->vertices = vertices;
}

float CustomShape::calculateArea() const {
    return calculatePolygonArea();
}

float CustomShape::calculatePolygonArea() const {
    // Polygon area calculation using the Shoelace formula
    float area = 0;
    int n = vertices.size();
    for (int i = 0; i < n; i++) {
        Vector2D current = vertices[i];
        Vector2D next = vertices[(i + 1) % n];
        area += current.x * next.y - next.x * current.y;
    }
    return 0.5f * std::abs(area);
}
