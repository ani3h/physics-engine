// File: tests/java/test_jni_integration.java
package tests.java;

import JAVA.ObjectState;
import JAVA.jni.PhysicsEngineJNI;
import org.junit.Before;
import org.junit.After;
import org.junit.Test;
import static org.junit.Assert.*;

public class TestJNIIntegration {
    private long worldPtr;
    private static final double EPSILON = 0.0001;

    @Before
    public void setUp() {
        // Create a new physics world for each test
        worldPtr = PhysicsEngineJNI.createPhysicsWorld();
        assertNotEquals("Physics world creation failed", 0, worldPtr);
    }

    @After
    public void tearDown() {
        // Clean up the physics world after each test
        PhysicsEngineJNI.deletePhysicsWorld(worldPtr);
    }

    @Test
    public void testWorldCreationAndDeletion() {
        // Test multiple world creation and deletion
        long[] worlds = new long[5];
        for (int i = 0; i < worlds.length; i++) {
            worlds[i] = PhysicsEngineJNI.createPhysicsWorld();
            assertNotEquals("World creation failed for index " + i, 0, worlds[i]);
        }

        // Clean up all created worlds
        for (long ptr : worlds) {
            PhysicsEngineJNI.deletePhysicsWorld(ptr);
        }
    }

    @Test
    public void testObjectCreation() {
        // Test Rectangle creation
        double[] rectDimensions = {50.0, 30.0};
        PhysicsEngineJNI.addObject(worldPtr, 1, 10.0, 0.0, 0.0, 0.0, 0.0, 'R', rectDimensions);
        ObjectState rectState = PhysicsEngineJNI.getObjectState(worldPtr, 1);
        assertNotNull("Rectangle creation failed", rectState);
        assertEquals("Rectangle ID mismatch", 1, rectState.getId());

        // Test Circle creation
        double[] circleDimensions = {20.0};
        PhysicsEngineJNI.addObject(worldPtr, 2, 5.0, 100.0, 100.0, 0.0, 0.0, 'C', circleDimensions);
        ObjectState circleState = PhysicsEngineJNI.getObjectState(worldPtr, 2);
        assertNotNull("Circle creation failed", circleState);
        assertEquals("Circle ID mismatch", 2, circleState.getId());

        // Test Square creation
        double[] squareDimensions = {40.0};
        PhysicsEngineJNI.addObject(worldPtr, 3, 15.0, 50.0, 50.0, 0.0, 0.0, 'S', squareDimensions);
        ObjectState squareState = PhysicsEngineJNI.getObjectState(worldPtr, 3);
        assertNotNull("Square creation failed", squareState);
        assertEquals("Square ID mismatch", 3, squareState.getId());
    }

    @Test
    public void testPhysicsSimulation() {
        // Create a test object
        double[] dimensions = {10.0};
        PhysicsEngineJNI.addObject(worldPtr, 1, 1.0, 0.0, 0.0, 0.0, 0.0, 'S', dimensions);

        // Get initial state
        ObjectState initialState = PhysicsEngineJNI.getObjectState(worldPtr, 1);
        assertNotNull("Failed to get initial state", initialState);

        // Step simulation
        PhysicsEngineJNI.stepSimulation(worldPtr, 1.0);

        // Get updated state
        ObjectState updatedState = PhysicsEngineJNI.getObjectState(worldPtr, 1);
        assertNotNull("Failed to get updated state", updatedState);

        // Verify gravity effect (object should fall)
        assertTrue("Gravity not applied correctly",
                  updatedState.getPosY() > initialState.getPosY());
    }

    @Test
    public void testForceConfiguration() {
        // Configure gravity
        double[] gravityParams = {5.0};
        PhysicsEngineJNI.configureForces(worldPtr, 2, gravityParams);

        // Add test object
        double[] dimensions = {10.0};
        PhysicsEngineJNI.addObject(worldPtr, 1, 1.0, 0.0, 0.0, 0.0, 0.0, 'S', dimensions);

        // Step simulation
        PhysicsEngineJNI.stepSimulation(worldPtr, 1.0);

        // Get state and verify modified gravity effect
        ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, 1);
        assertNotNull(state);
        assertTrue("Modified gravity not applied correctly",
                  state.getAccY() > 0);

        // Test friction configuration
        double[] frictionParams = {0.5, 0.3};
        PhysicsEngineJNI.configureForces(worldPtr, 1, frictionParams);
    }

    @Test
    public void testCollisionHandling() {
        // Create two objects on collision course
        double[] dimensions = {10.0};
        PhysicsEngineJNI.addObject(worldPtr, 1, 1.0, 0.0, 0.0, 1.0, 0.0, 'S', dimensions);
        PhysicsEngineJNI.addObject(worldPtr, 2, 1.0, 20.0, 0.0, -1.0, 0.0, 'S', dimensions);

        // Get initial velocities
        ObjectState initial1 = PhysicsEngineJNI.getObjectState(worldPtr, 1);
        ObjectState initial2 = PhysicsEngineJNI.getObjectState(worldPtr, 2);

        // Step simulation until collision occurs
        for (int i = 0; i < 10; i++) {
            PhysicsEngineJNI.stepSimulation(worldPtr, 0.1);
            PhysicsEngineJNI.handleCollisions(worldPtr);
        }

        // Get final states
        ObjectState final1 = PhysicsEngineJNI.getObjectState(worldPtr, 1);
        ObjectState final2 = PhysicsEngineJNI.getObjectState(worldPtr, 2);

        // Verify velocities changed due to collision
        assertNotEquals("Collision not detected - velocity unchanged",
                       initial1.getVelX(), final1.getVelX(), EPSILON);
        assertNotEquals("Collision not detected - velocity unchanged",
                       initial2.getVelX(), final2.getVelX(), EPSILON);
    }

    @Test
    public void testObjectStateUpdates() {
        // Create test object
        double[] dimensions = {10.0};
        PhysicsEngineJNI.addObject(worldPtr, 1, 1.0, 0.0, 0.0, 0.0, 0.0, 'S', dimensions);

        // Update object state
        double newPosX = 50.0;
        double newPosY = 50.0;
        double newVelX = 2.0;
        double newVelY = -2.0;
        PhysicsEngineJNI.updateObjectState(worldPtr, 1, newPosX, newPosY, newVelX, newVelY);

        // Verify state update
        ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, 1);
        assertNotNull("Failed to get updated state", state);
        assertEquals("Position X not updated", newPosX, state.getPosX(), EPSILON);
        assertEquals("Position Y not updated", newPosY, state.getPosY(), EPSILON);
        assertEquals("Velocity X not updated", newVelX, state.getVelX(), EPSILON);
        assertEquals("Velocity Y not updated", newVelY, state.getVelY(), EPSILON);
    }

    @Test
    public void testErrorHandling() {
        // Test invalid object ID
        ObjectState invalidState = PhysicsEngineJNI.getObjectState(worldPtr, 999);
        assertNull("Invalid object ID should return null state", invalidState);

        // Test invalid shape type
        double[] dimensions = {10.0};
        try {
            PhysicsEngineJNI.addObject(worldPtr, 1, 1.0, 0.0, 0.0, 0.0, 0.0, 'X', dimensions);
        } catch (Exception e) {
            // Expected behavior for invalid shape type
            return;
        }
        fail("Invalid shape type should throw exception");
    }

    @Test
    public void testConcurrentAccess() {
        // Create test objects
        double[] dimensions = {10.0};
        for (int i = 1; i <= 5; i++) {
            PhysicsEngineJNI.addObject(worldPtr, i, 1.0, i * 20.0, 0.0, 0.0, 0.0, 'S', dimensions);
        }

        // Test concurrent access to physics world
        Thread[] threads = new Thread[3];
        for (int i = 0; i < threads.length; i++) {
            threads[i] = new Thread(() -> {
                for (int j = 0; j < 100; j++) {
                    PhysicsEngineJNI.stepSimulation(worldPtr, 0.016);
                    PhysicsEngineJNI.handleCollisions(worldPtr);
                }
            });
            threads[i].start();
        }

        // Wait for all threads to complete
        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                fail("Thread interrupted: " + e.getMessage());
            }
        }
    }
}