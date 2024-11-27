// File: tests/java/test_gui_interaction.java
package tests.java;

import JAVA.GUIController;
import JAVA.PhysicsSimulation;
import JAVA.jni.PhysicsEngineJNI;
import javafx.scene.canvas.Canvas;
import javafx.scene.paint.Color;
import org.junit.Before;
import org.junit.After;
import org.junit.Test;
import static org.junit.Assert.*;

public class TestGUIInteraction {
    private GUIController controller;
    private Canvas canvas;
    private PhysicsSimulation simulation;
    private long worldPtr;

    @Before
    public void setUp() {
        // Initialize JavaFX components and physics world
        canvas = new Canvas(800, 600);
        simulation = new PhysicsSimulation();
        worldPtr = PhysicsEngineJNI.createPhysicsWorld();
        controller = new GUIController(canvas, simulation, worldPtr);
    }

    @After
    public void tearDown() {
        // Clean up resources
        controller.cleanup();
        PhysicsEngineJNI.deletePhysicsWorld(worldPtr);
    }

    @Test
    public void testSimulationControls() {
        // Test simulation start
        assertFalse("Simulation should be stopped initially", controller.isRunning());
        
        controller.startSimulation();
        assertTrue("Simulation should be running after start", controller.isRunning());
        
        controller.pauseSimulation();
        assertFalse("Simulation should be paused", controller.isRunning());
        
        controller.resetSimulation();
        assertFalse("Simulation should be stopped after reset", controller.isRunning());
    }

    @Test
    public void testObjectAddition() {
        // Test adding different shapes
        controller.handleAddObject("Rectangle");
        controller.handleAddObject("Circle");
        controller.handleAddObject("Square");
        
        // Note: Since objectShapes is private, we can verify object addition
        // indirectly by checking simulation behavior
        controller.startSimulation();
        assertTrue("Simulation should run with added objects", controller.isRunning());
    }

    @Test
    public void testVectorDisplay() {
        // Test vector display toggle
        controller.toggleVectorDisplay();
        controller.toggleVectorDisplay(); // Toggle back
        
        // Note: Visual verification would be needed for complete testing
        // Here we're just ensuring the method doesn't throw exceptions
    }

    @Test
    public void testUpdateMethod() {
        // Add a test object
        controller.handleAddObject("Circle");
        
        // Test physics update
        controller.startSimulation();
        controller.update(0.016); // Simulate one frame at 60fps
        
        // Note: Exact position testing would require access to private fields
        // Here we're just ensuring the update method runs without exceptions
    }

    @Test
    public void testGroundCollisionHandling() {
        // Add an object that will fall
        controller.handleAddObject("Square");
        
        // Run simulation for a few frames
        controller.startSimulation();
        for (int i = 0; i < 60; i++) { // Simulate 1 second at 60fps
            controller.update(0.016);
        }
        
        // Note: Visual verification would be needed for complete testing
        // Here we're ensuring multiple updates don't cause errors
    }

    @Test
    public void testSimulationReset() {
        // Add some objects
        controller.handleAddObject("Rectangle");
        controller.handleAddObject("Circle");
        
        // Start and run simulation
        controller.startSimulation();
        controller.update(0.016);
        
        // Reset simulation
        controller.resetSimulation();
        
        // Verify simulation state
        assertFalse("Simulation should be stopped after reset", controller.isRunning());
    }
}