#ifndef PHYSICS_NATIVE_H
#define PHYSICS_NATIVE_H

#include <jni.h>
#include "vector2d.h"
#include "object.h"
#include "forces.h"
#include "collider.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     JAVA_jni_PhysicsEngineJNI
 * Method:    createPhysicsWorld
 */
JNIEXPORT jlong JNICALL Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld
  (JNIEnv *, jclass);

/*
 * Class:     JAVA_jni_PhysicsEngineJNI
 * Method:    deletePhysicsWorld
 */
JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld
  (JNIEnv *, jclass, jlong);

/*
 * Class:     JAVA_jni_PhysicsEngineJNI
 * Method:    addObject
 */
JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_addObject
  (JNIEnv *, jclass, jlong, jint, jdouble, jdouble, jdouble, jdouble, jdouble, jchar, jdoubleArray);

/*
 * Class:     JAVA_jni_PhysicsEngineJNI
 * Method:    handleCollisions
 */
JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_handleCollisions
  (JNIEnv *, jclass, jlong);

/*
 * Class:     JAVA_jni_PhysicsEngineJNI
 * Method:    configureForces
 */
JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_configureForces
  (JNIEnv *, jclass, jlong, jint, jdoubleArray);

/*
 * Class:     JAVA_jni_PhysicsEngineJNI
 * Method:    stepSimulation
 */
JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_stepSimulation
  (JNIEnv *, jclass, jlong, jdouble);

/*
 * Class:     JAVA_jni_PhysicsEngineJNI
 * Method:    getObjectState
 */
JNIEXPORT jobject JNICALL Java_JAVA_jni_PhysicsEngineJNI_getObjectState
  (JNIEnv *, jclass, jlong, jint);

/*
 * Class:     JAVA_jni_PhysicsEngineJNI
 * Method:    updateObjectState
 */
JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_updateObjectState
  (JNIEnv *, jclass, jlong, jint, jdouble, jdouble, jdouble, jdouble);

/*
 * Struct definition for PhysicsWorld
 */
struct PhysicsWorld {
    std::vector<Object*> objects;
    float gravity;
    float staticFriction;
    float kineticFriction;
    double groundLevel;
    const double GROUND_THRESHOLD = 0.1;
    const double VELOCITY_THRESHOLD = 0.01;

    PhysicsWorld() : 
        gravity(9.81f),
        staticFriction(0.5f),
        kineticFriction(0.3f),
        groundLevel(600.0) {} // Assuming 600 is your canvas height - 5
};

/*
 * Helper function declarations
 */
bool detectCollision(Object* objA, Object* objB);
void resolveCollision(Object* objA, Object* objB);
void applyForces(Object* obj, const PhysicsWorld* world);
void updateObjectPhysics(Object* obj, double deltaTime);
bool isNearGround(const Object* obj, const PhysicsWorld* world);

#ifdef __cplusplus
}
#endif

#endif // PHYSICS_NATIVE_H