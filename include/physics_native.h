#ifndef PHYSICS_NATIVE_H
#define PHYSICS_NATIVE_H

#include <jni.h>
#include "vector2d.h"
#include "object.h"
#include "forces.h"
#include "collider.h"

// Physics World struct to hold simulation state
struct PhysicsWorld {
    std::vector<Object*> objects;
    Forces forces;
    float gravity;
    float staticFriction;
    float kineticFriction;
};

#ifdef __cplusplus
extern "C" {
#endif

// JNI method declarations
JNIEXPORT jlong JNICALL Java_JAVA_jni_PhysicsEngineJNI_createPhysicsWorld
  (JNIEnv *, jclass);

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_deletePhysicsWorld
  (JNIEnv *, jclass, jlong);

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_addObject
  (JNIEnv *, jclass, jlong, jint, jdouble, jdouble, jdouble, jdouble, jdouble, jchar, jdoubleArray);

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_handleCollisions
  (JNIEnv *, jclass, jlong);

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_configureForces
  (JNIEnv *, jclass, jlong, jint, jdoubleArray);

JNIEXPORT void JNICALL Java_JAVA_jni_PhysicsEngineJNI_stepSimulation
  (JNIEnv *, jclass, jlong, jdouble);

JNIEXPORT jobject JNICALL Java_JAVA_jni_PhysicsEngineJNI_getObjectState
  (JNIEnv *, jclass, jlong, jint);

#ifdef __cplusplus
}
#endif

#endif // PHYSICS_NATIVE_H