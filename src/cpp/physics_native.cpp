#include "./include/physics_native.h"
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <vector>

// Basic PhysicsWorld structure to hold simulation state
struct PhysicsWorld {
    std::vector<Object*> objects;
    float gravity;
    float staticFriction;
    float kineticFriction;
};

JNIEXPORT jlong JNICALL Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld
  (JNIEnv* env, jclass)
{
    auto* world = new PhysicsWorld();
    world->gravity = 9.81f;
    world->staticFriction = 0.5f;
    world->kineticFriction = 0.3f;
    return reinterpret_cast<jlong>(world);
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
            case 'R': // Rectangle
                if (dimsLength >= 2) {
                    obj = new Rectangle(id, mass, position, velocity, dims[0], dims[1]);
                }
                break;
            case 'C': // Circle
                if (dimsLength >= 1) {
                    obj = new Circle(id, mass, position, velocity, dims[0]);
                }
                break;
            case 'S': // Square
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
    
    // Check collisions between all pairs of objects
    for (size_t i = 0; i < world->objects.size(); i++) {
        for (size_t j = i + 1; j < world->objects.size(); j++) {
            Object* objA = world->objects[i];
            Object* objB = world->objects[j];
            
            // Get colliders
            Collider* colliderA = objA->getCollider();
            Collider* colliderB = objB->getCollider();
            
            if (colliderA && colliderB && colliderA->checkCollision(*colliderB)) {
                colliderA->resolveCollision(*objA, *objB);
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
        case 1: // Friction coefficients
            if (length >= 2) {
                world->staticFriction = values[0];
                world->kineticFriction = values[1];
            }
            break;
        case 2: // Gravity
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
        // Apply forces
        Forces::applyGravity(*obj, world->gravity);
        Vector2D surfaceNormal(0, 1);  // Assuming ground is below
        Forces::applyStaticFriction(*obj, world->staticFriction, surfaceNormal);
        Forces::applyKineticFriction(*obj, world->kineticFriction, surfaceNormal);
        
        // Update object state
        obj->update(deltaTime);
        obj->resetForces();
    }
}

JNIEXPORT jobject JNICALL Java_JAVA_jni_PhysicsEngineJNI_getObjectState
  (JNIEnv* env, jclass, jlong worldPtr, jint objectId)
{
    auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
    
    // Find object with given ID
    auto it = std::find_if(world->objects.begin(), world->objects.end(),
        [objectId](const Object* obj) { return obj->getID() == objectId; });
    
    if (it != world->objects.end()) {
        Object* obj = *it;
        
        // Find ObjectState class
        jclass objectStateClass = env->FindClass("JAVA/ObjectState");
        if (!objectStateClass) return nullptr;
        
        // Get constructor
        jmethodID constructor = env->GetMethodID(objectStateClass, "<init>", "(IDDDDDD)V");  // ID, posX, posY, velX, velY, accX, accY
        if (!constructor) return nullptr;
        
        // Create and return new ObjectState object
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
