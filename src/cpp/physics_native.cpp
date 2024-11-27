#include "physics_native.h"
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <vector>

// Helper function implementations
bool detectCollision(Object* objA, Object* objB) {
    Collider* colliderA = objA->getCollider();
    Collider* colliderB = objB->getCollider();
    
    return (colliderA && colliderB && colliderA->checkCollision(*colliderB));
}

void resolveCollision(Object* objA, Object* objB) {
    Collider* colliderA = objA->getCollider();
    if (colliderA) {
        colliderA->resolveCollision(*objA, *objB);
    }
}

void applyForces(Object* obj, const PhysicsWorld* world) {
    // Apply gravity
    Forces::applyGravity(*obj, world->gravity);
    
    // Apply friction if near ground
    if (isNearGround(obj, world)) {
        Vector2D surfaceNormal(0, 1);
        double velocityMagnitude = obj->getVelocity().length();
        
        if (velocityMagnitude < world->VELOCITY_THRESHOLD) {
            Forces::applyStaticFriction(*obj, world->staticFriction, surfaceNormal);
        } else {
            Forces::applyKineticFriction(*obj, world->kineticFriction, surfaceNormal);
        }
    }
}

void updateObjectPhysics(Object* obj, double deltaTime) {
    obj->update(deltaTime);
}

bool isNearGround(const Object* obj, const PhysicsWorld* world) {
    return std::abs(obj->getPosition().y - world->groundLevel) < world->GROUND_THRESHOLD;
}

// JNI implementations
JNIEXPORT jlong JNICALL Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld
  (JNIEnv* env, jclass)
{
    return reinterpret_cast<jlong>(new PhysicsWorld());
}

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld
  (JNIEnv* env, jclass, jlong worldPtr)
{
    auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    for (auto* obj : world->objects) {
        delete obj;
    }
    delete world;
}

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_addObject
  (JNIEnv* env, jclass, jlong worldPtr, jint id, jdouble mass, 
   jdouble posX, jdouble posY, jdouble velX, jdouble velY,
   jchar shape, jdoubleArray dimensions)
{
    auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    jdouble* dims = env->GetDoubleArrayElements(dimensions, nullptr);
    jsize dimsLength = env->GetArrayLength(dimensions);
    
    Vector2D position(posX, posY);
    Vector2D velocity(velX, velY);
    Object* obj = nullptr;

    try {
        switch (shape) {
            case 'R':
                if (dimsLength >= 2) {
                    obj = new Rectangle(id, mass, position, velocity, dims[0], dims[1]);
                }
                break;
            case 'C':
                if (dimsLength >= 1) {
                    obj = new Circle(id, mass, position, velocity, dims[0]);
                }
                break;
            case 'S':
                if (dimsLength >= 1) {
                    obj = new Square(id, mass, position, velocity, dims[0]);
                }
                break;
        }
        
        if (obj) {
            world->objects.push_back(obj);
        }
    } catch (const std::exception& e) {
        // Handle exception
    }

    env->ReleaseDoubleArrayElements(dimensions, dims, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_handleCollisions
  (JNIEnv* env, jclass, jlong worldPtr)
{
    auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    const int maxIterations = 4; // Maximum iterations for collision resolution
    
    for (int iteration = 0; iteration < maxIterations; iteration++) {
        std::vector<std::pair<Object*, Object*>> collidingPairs;
        
        // First pass: Detect all collisions
        for (size_t i = 0; i < world->objects.size(); i++) {
            for (size_t j = i + 1; j < world->objects.size(); j++) {
                if (detectCollision(world->objects[i], world->objects[j])) {
                    collidingPairs.push_back({world->objects[i], world->objects[j]});
                }
            }
        }
        
        if (collidingPairs.empty()) {
            break; // No more collisions to resolve
        }
        
        // Second pass: Resolve collisions
        for (const auto& pair : collidingPairs) {
            Collider* colliderA = pair.first->getCollider();
            Collider* colliderB = pair.second->getCollider();
            
            if (colliderA && colliderB) {
                // Handle all collider type combinations
                if (auto* aabbA = dynamic_cast<AABBCollider*>(colliderA)) {
                    if (auto* aabbB = dynamic_cast<AABBCollider*>(colliderB)) {
                        // AABB vs AABB collision
                        aabbA->resolveCollision(*pair.first, *pair.second);
                    } else if (auto* circleB = dynamic_cast<CircleCollider*>(colliderB)) {
                        // AABB vs Circle collision
                        aabbA->resolveCollision(*pair.first, *pair.second);
                    }
                } else if (auto* circleA = dynamic_cast<CircleCollider*>(colliderA)) {
                    if (auto* circleB = dynamic_cast<CircleCollider*>(colliderB)) {
                        // Circle vs Circle collision
                        circleA->resolveCollision(*pair.first, *pair.second);
                    } else if (auto* aabbB = dynamic_cast<AABBCollider*>(colliderB)) {
                        // Circle vs AABB collision
                        circleA->resolveCollision(*pair.first, *pair.second);
                    }
                }
            }
        }
    }
}

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_configureForces
  (JNIEnv* env, jclass, jlong worldPtr, jint choice, jdoubleArray params)
{
    auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    jdouble* values = env->GetDoubleArrayElements(params, nullptr);
    jsize length = env->GetArrayLength(params);

    switch (choice) {
        case 1:
            if (length >= 2) {
                world->staticFriction = values[0];
                world->kineticFriction = values[1];
            }
            break;
        case 2:
            if (length >= 1) {
                world->gravity = values[0];
            }
            break;
    }

    env->ReleaseDoubleArrayElements(params, values, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_stepSimulation
  (JNIEnv* env, jclass, jlong worldPtr, jdouble deltaTime)
{
    auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    
    for (auto* obj : world->objects) {
        obj->resetForces();
        applyForces(obj, world);
        updateObjectPhysics(obj, deltaTime);
    }
}

JNIEXPORT jobject JNICALL Java_JAVA_jni_PhysicsEngineJNI_getObjectState
  (JNIEnv* env, jclass, jlong worldPtr, jint objectId)
{
    auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    
    auto it = std::find_if(world->objects.begin(), world->objects.end(),
        [objectId](const Object* obj) { return obj->getID() == objectId; });
    
    if (it != world->objects.end()) {
        Object* obj = *it;
        
        jclass objectStateClass = env->FindClass("JAVA/ObjectState");
        if (!objectStateClass) return nullptr;
        
        jmethodID constructor = env->GetMethodID(objectStateClass, "<init>", "(IDDDDDD)V");
        if (!constructor) return nullptr;
        
        const Vector2D& pos = obj->getPosition();
        const Vector2D& vel = obj->getVelocity();
        const Vector2D& acc = obj->getAcceleration();
        
        return env->NewObject(objectStateClass, constructor,
            obj->getID(),
            pos.x, pos.y,
            vel.x, vel.y,
            acc.x, acc.y);
    }
    
    return nullptr;
}

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_updateObjectState
  (JNIEnv* env, jclass, jlong worldPtr, jint objectId, jdouble posX, jdouble posY, 
   jdouble velX, jdouble velY)
{
    auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    
    auto it = std::find_if(world->objects.begin(), world->objects.end(),
        [objectId](const Object* obj) { return obj->getID() == objectId; });
    
    if (it != world->objects.end()) {
        Object* obj = *it;
        obj->setPosition(Vector2D(posX, posY));
        obj->setVelocity(Vector2D(velX, velY));

        // Update the collider position
        Collider* collider = obj->getCollider();
        if (collider) {
            AABBCollider* aabbCollider = dynamic_cast<AABBCollider*>(collider);
            if (aabbCollider) {
                aabbCollider->position = Vector2D(posX, posY);
            }
            CircleCollider* circleCollider = dynamic_cast<CircleCollider*>(collider);
            if (circleCollider) {
                circleCollider->center = Vector2D(posX, posY);
            }
        }
    }
}