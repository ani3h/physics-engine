package JAVA;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.control.Alert;
import javafx.scene.control.TextInputDialog;
import JAVA.jni.PhysicsEngineJNI;
import java.util.Optional;
import java.util.Random;

public class GUIController {
    private Canvas canvas;
    private GraphicsContext gc;
    private PhysicsSimulation simulation;
    private long worldPtr;
    private boolean isRunning;
    private int nextId = 1;
    private Random random;

    public GUIController(Canvas canvas, PhysicsSimulation simulation, long worldPtr) {
        this.canvas = canvas;
        this.gc = canvas.getGraphicsContext2D();
        this.simulation = simulation;
        this.worldPtr = worldPtr;
        this.isRunning = false;
        this.random = new Random();
        
        // Set up canvas click handler for object placement
        canvas.setOnMouseClicked(e -> {
            if (!isRunning) {
                double x = e.getX();
                double y = e.getY();
                showAddObjectDialog(x, y);
            }
        });

        // Initial canvas setup
        clearCanvas();
    }

    public void update(double deltaTime) {
        if (isRunning) {
            // Step the physics simulation
            PhysicsEngineJNI.stepSimulation(worldPtr, deltaTime);
            
            // Handle any collisions
            PhysicsEngineJNI.handleCollisions(worldPtr);
            
            // Redraw the canvas
            render();
        }
    }

    private void render() {
        clearCanvas();
        // TODO: Get object positions and properties from the physics engine and render them
        // This would require additional JNI methods to get object states
        
        // For now, we'll just draw placeholder shapes
        // In a real implementation, you would get the actual object data from the physics engine
        gc.setFill(Color.BLUE);
        gc.fillRect(100, 100, 50, 50); // Example rectangle
        gc.setFill(Color.RED);
        gc.fillOval(200, 200, 40, 40); // Example circle
    }

    private void clearCanvas() {
        gc.setFill(Color.WHITE);
        gc.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());
        gc.setStroke(Color.BLACK);
        gc.strokeRect(0, 0, canvas.getWidth(), canvas.getHeight());
    }

    public void handleAddObject(String shapeType) {
        double defaultMass = 1.0;
        double[] dimensions;
        char shapeChar;

        switch (shapeType) {
            case "Rectangle":
                dimensions = new double[]{50.0, 30.0}; // width, height
                shapeChar = 'R';
                break;
            case "Circle":
                dimensions = new double[]{20.0}; // radius
                shapeChar = 'C';
                break;
            case "Square":
                dimensions = new double[]{40.0}; // side length
                shapeChar = 'S';
                break;
            default:
                showError("Invalid shape type");
                return;
        }

        // Generate random position within canvas bounds
        double posX = random.nextDouble() * (canvas.getWidth() - 50);
        double posY = random.nextDouble() * (canvas.getHeight() - 50);
        
        // Initial velocity (can be modified through UI if needed)
        double velX = 0;
        double velY = 0;

        try {
            PhysicsEngineJNI.addObject(worldPtr, nextId++, defaultMass, 
                                     posX, posY, velX, velY, 
                                     shapeChar, dimensions);
            render();
        } catch (Exception e) {
            showError("Failed to add object: " + e.getMessage());
        }
    }

    private void showAddObjectDialog(double x, double y) {
        TextInputDialog dialog = new TextInputDialog("1.0");
        dialog.setTitle("Add Object");
        dialog.setHeaderText("Enter object mass:");
        dialog.setContentText("Mass:");

        Optional<String> result = dialog.showAndWait();
        result.ifPresent(mass -> {
            try {
                double objectMass = Double.parseDouble(mass);
                // Add object at clicked position
                PhysicsEngineJNI.addObject(worldPtr, nextId++, objectMass, 
                                         x, y, 0, 0, 
                                         'R', new double[]{50.0, 30.0});
                render();
            } catch (NumberFormatException e) {
                showError("Invalid mass value");
            } catch (Exception e) {
                showError("Failed to add object: " + e.getMessage());
            }
        });
    }

    private void showError(String message) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("Error");
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }

    public void startSimulation() {
        isRunning = true;
    }

    public void pauseSimulation() {
        isRunning = false;
    }

    public void resetSimulation() {
        isRunning = false;
        // TODO: Implement reset logic in JNI
        clearCanvas();
        nextId = 1;
    }

    public boolean isRunning() {
        return isRunning;
    }

    public void cleanup() {
        // Add any necessary cleanup code here
        isRunning = false;
    }
}
