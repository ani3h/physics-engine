#include <jni.h>
#include "physics_world.h"
#include "object.h"
#include "forces.h"
#include <memory>
#include <stdexcept>

// Helper function to convert jstring to std::string
std::string jstring2string(JNIEnv* env, jstring jStr) {
    if (!jStr) return "";
    const char* cStr = env->GetStringUTFChars(jStr, nullptr);
    std::string str(cStr);
    env->ReleaseStringUTFChars(jStr, cStr);
    return str;
}

// Implementation of native methods

extern "C" {

JNIEXPORT jlong JNICALL Java_PhysicsSimulation_createPhysicsWorld
  (JNIEnv* env, jobject obj) {
    try {
        auto* world = new PhysicsWorld();
        return reinterpret_cast<jlong>(world);
    } catch (const std::exception& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), 
            ("Failed to create physics world: " + std::string(e.what())).c_str());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_PhysicsSimulation_deletePhysicsWorld
  (JNIEnv* env, jobject obj, jlong worldPtr) {
    try {
        auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
        delete world;
    } catch (const std::exception& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), 
            ("Failed to delete physics world: " + std::string(e.what())).c_str());
    }
}

JNIEXPORT void JNICALL Java_PhysicsSimulation_addObject
  (JNIEnv* env, jobject obj, jlong worldPtr, jint id, jdouble mass, 
   jdouble posX, jdouble posY, jdouble velX, jdouble velY, 
   jchar shape, jdoubleArray dimensions) {
    try {
        auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
        Vector2D position(posX, posY);
        Vector2D velocity(velX, velY);
        
        // Get dimensions array
        jsize len = env->GetArrayLength(dimensions);
        jdouble* dims = env->GetDoubleArrayElements(dimensions, nullptr);
        
        Object* newObject = nullptr;
        
        switch (shape) {
            case 'R': // Rectangle
                if (len >= 2) {
                    newObject = new Rectangle(id, mass, position, velocity, 
                                           dims[0], dims[1]);
                }
                break;
                
            case 'C': // Circle
                if (len >= 1) {
                    newObject = new Circle(id, mass, position, velocity, 
                                         dims[0]);
                }
                break;
                
            case 'S': // Square
                if (len >= 1) {
                    newObject = new Square(id, mass, position, velocity, 
                                         dims[0]);
                }
                break;
        }
        
        env->ReleaseDoubleArrayElements(dimensions, dims, JNI_ABORT);
        
        if (newObject) {
            world->addObject(newObject);
        } else {
            throw std::runtime_error("Failed to create object with given parameters");
        }
        
    } catch (const std::exception& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), 
            ("Failed to add object: " + std::string(e.what())).c_str());
    }
}

JNIEXPORT void JNICALL Java_PhysicsSimulation_configureForces
  (JNIEnv* env, jobject obj, jlong worldPtr, jint choice, jdoubleArray params) {
    try {
        auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
        jsize len = env->GetArrayLength(params);
        jdouble* values = env->GetDoubleArrayElements(params, nullptr);
        
        switch (choice) {
            case 1: // Friction coefficients
                if (len >= 2) {
                    // Set static and kinetic friction
                    for (auto* obj : world->getObjects()) {
                        Forces::applyStaticFriction(*obj, values[0], Vector2D(0, 1));
                        Forces::applyKineticFriction(*obj, values[1], Vector2D(0, 1));
                    }
                }
                break;
                
            case 2: // Gravity
                if (len >= 1) {
                    // Update gravity for all objects
                    for (auto* obj : world->getObjects()) {
                        Forces::applyGravity(*obj, values[0]);
                    }
                }
                break;
        }
        
        env->ReleaseDoubleArrayElements(params, values, JNI_ABORT);
        
    } catch (const std::exception& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), 
            ("Failed to configure forces: " + std::string(e.what())).c_str());
    }
}

JNIEXPORT void JNICALL Java_PhysicsSimulation_stepSimulation
  (JNIEnv* env, jobject obj, jlong worldPtr, jdouble deltaTime) {
    try {
        auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
        world->step(deltaTime);
    } catch (const std::exception& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), 
            ("Failed to step simulation: " + std::string(e.what())).c_str());
    }
}

JNIEXPORT void JNICALL Java_PhysicsSimulation_handleCollisions
  (JNIEnv* env, jobject obj, jlong worldPtr) {
    try {
        auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
        world->handleCollisions();
    } catch (const std::exception& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), 
            ("Failed to handle collisions: " + std::string(e.what())).c_str());
    }
}

JNIEXPORT void JNICALL Java_PhysicsSimulation_displayObjectInfo
  (JNIEnv* env, jobject obj, jlong worldPtr, jint objectId) {
    try {
        auto* world = reinterpret_cast<PhysicsWorld*>(worldPtr);
        const auto& objects = world->getObjects();
        
        auto it = std::find_if(objects.begin(), objects.end(),
            [objectId](const Object* obj) { return obj->getID() == objectId; });
            
        if (it != objects.end()) {
            const Object* object = *it;
            const Vector2D& pos = object->getPosition();
            const Vector2D& vel = object->getVelocity();
            
            // Create formatted string
            std::string info = "Object " + std::to_string(objectId) + ":\n"
                             "Position: (" + std::to_string(pos.x) + ", " 
                             + std::to_string(pos.y) + ")\n"
                             "Velocity: (" + std::to_string(vel.x) + ", " 
                             + std::to_string(vel.y) + ")\n"
                             "Mass: " + std::to_string(object->getMass());
                             
            // Convert to Java string
            jstring jinfo = env->NewStringUTF(info.c_str());
            
            // Get System.out.println
            jclass system = env->FindClass("java/lang/System");
            jfieldID outField = env->GetStaticFieldID(system, "out", 
                "Ljava/io/PrintStream;");
            jobject out = env->GetStaticObjectField(system, outField);
            jclass printStream = env->FindClass("java/io/PrintStream");
            jmethodID println = env->GetMethodID(printStream, "println", 
                "(Ljava/lang/String;)V");
            
            // Print info
            env->CallVoidMethod(out, println, jinfo);
        }
    } catch (const std::exception& e) {
        env->ThrowNew(env->FindClass("java/lang/RuntimeException"), 
            ("Failed to display object info: " + std::string(e.what())).c_str());
    }
}

} // extern "C"
