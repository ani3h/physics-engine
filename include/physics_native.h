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

#ifdef __cplusplus
}
#endif

#endif // PHYSICS_NATIVE_H