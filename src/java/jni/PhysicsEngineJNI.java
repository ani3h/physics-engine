// File: src/JAVA/jni/PhysicsEngineJNI.java
package JAVA.jni;

import JAVA.ObjectState;

public class PhysicsEngineJNI {
    static {
        try {
            // Try loading with platform-specific naming
            String osName = System.getProperty("os.name").toLowerCase();
            if (osName.contains("mac")) {
                try {
                    System.load(System.getProperty("user.dir") + "/lib/libphysics_native.dylib");
                } catch (UnsatisfiedLinkError e) {
                    System.loadLibrary("physics_native");
                }
            } else {
                System.loadLibrary("physics_native");
            }
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native library load failed: " + e.getMessage());
            System.err.println("Library path: " + System.getProperty("java.library.path"));
            System.err.println("Current directory: " + System.getProperty("user.dir"));
            throw e;
        }
    }

    // Native method declarations remain the same
    public static native long createPhysicsWorld();
    public static native void deletePhysicsWorld(long worldPtr);
    public static native void addObject(long worldPtr, int id, double mass, double posX, double posY, double velX, double velY, char shape, double... dimensions);
    public static native void handleCollisions(long worldPtr);
    public static native void configureForces(long worldPtr, int choice, double... params);
    public static native void stepSimulation(long worldPtr, double deltaTime);
    public static native void displayObjectInfo(long worldPtr, int objectId);
    public static native ObjectState getObjectState(long worldPtr, int objectId);
    public static native void updateObjectState(long worldPtr, int id, double posX, double posY, double velX, double velY);
}
