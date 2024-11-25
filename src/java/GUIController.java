package JAVA;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.control.Alert;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.ChoiceDialog;
import JAVA.jni.PhysicsEngineJNI;
import java.util.*;

public class GUIController {
    private Canvas canvas;
    private GraphicsContext gc;
    private PhysicsSimulation simulation;
    private long worldPtr;
    private boolean isRunning;
    private int nextId = 1;
    private Random random;
    private Map<Integer, ShapeInfo> objectShapes;  // Store shape information for each object
    private boolean isShowingVectors = false;  // Moved to class level variables

    private static class ShapeInfo {
        char type;
        double[] dimensions;
        Color color;

        ShapeInfo(char type, double[] dimensions, Color color) {
            this.type = type;
            this.dimensions = dimensions;
            this.color = color;
        }
    }

    public GUIController(Canvas canvas, PhysicsSimulation simulation, long worldPtr) {
        this.canvas = canvas;
        this.gc = canvas.getGraphicsContext2D();
        this.simulation = simulation;
        this.worldPtr = worldPtr;
        this.isRunning = false;
        this.random = new Random();
        this.objectShapes = new HashMap<>();
        
        canvas.setOnMouseClicked(e -> {
            if (!isRunning) {
                showAddObjectDialog(e.getX(), e.getY());
            }
        });

        clearCanvas();
    }

    private void clearCanvas() {
        gc.setFill(Color.WHITE);
        gc.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());
        gc.setStroke(Color.BLACK);
        gc.strokeRect(0, 0, canvas.getWidth(), canvas.getHeight());
    }

    public void update(double deltaTime) {
        if (isRunning) {
            PhysicsEngineJNI.stepSimulation(worldPtr, deltaTime);
            PhysicsEngineJNI.handleCollisions(worldPtr);
            render();
        }
    }

    private void render() {
        clearCanvas();
        
        // Render each object based on its current state
        for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
            int id = entry.getKey();
            ShapeInfo shapeInfo = entry.getValue();
            
            // Get current state from physics engine
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
            if (state == null) continue;
            
            gc.setFill(shapeInfo.color);
            
            switch (shapeInfo.type) {
                case 'R': // Rectangle
                    gc.fillRect(state.getPosX(), state.getPosY(),
                              shapeInfo.dimensions[0], 
                              shapeInfo.dimensions[1]);
                    break;
                case 'C': // Circle
                    gc.fillOval(state.getPosX(), state.getPosY(),
                              shapeInfo.dimensions[0] * 2, 
                              shapeInfo.dimensions[0] * 2);
                    break;
                case 'S': // Square
                    gc.fillRect(state.getPosX(), state.getPosY(),
                              shapeInfo.dimensions[0], 
                              shapeInfo.dimensions[0]);
                    break;
            }
            
            // Draw velocity vector (optional visualization)
            if (isShowingVectors) {
                gc.setStroke(Color.RED);
                gc.setLineWidth(2);
                double startX = state.getPosX();
                double startY = state.getPosY();
                double endX = startX + state.getVelX() * 10;
                double endY = startY + state.getVelY() * 10;
                gc.strokeLine(startX, startY, endX, endY);
                
                // Draw arrowhead
                double angle = Math.atan2(endY - startY, endX - startX);
                double arrowLength = 10;
                gc.strokeLine(endX, endY, 
                            endX - arrowLength * Math.cos(angle - Math.PI/6),
                            endY - arrowLength * Math.sin(angle - Math.PI/6));
                gc.strokeLine(endX, endY,
                            endX - arrowLength * Math.cos(angle + Math.PI/6),
                            endY - arrowLength * Math.sin(angle + Math.PI/6));
            }
        }
    }

    public void handleAddObject(String shapeType) {
        double defaultMass = 1.0;
        double[] dimensions;
        char shapeChar;
        Color color = Color.color(random.nextDouble(), random.nextDouble(), random.nextDouble());

        switch (shapeType.toUpperCase()) {
            case "RECTANGLE":
                dimensions = new double[]{50.0, 30.0}; // width, height
                shapeChar = 'R';
                break;
            case "CIRCLE":
                dimensions = new double[]{20.0}; // radius
                shapeChar = 'C';
                break;
            case "SQUARE":
                dimensions = new double[]{40.0}; // side length
                shapeChar = 'S';
                break;
            default:
                showError("Invalid shape type");
                return;
        }

        double posX = random.nextDouble() * (canvas.getWidth() - dimensions[0]);
        double posY = random.nextDouble() * (canvas.getHeight() - 
                     (shapeChar == 'C' ? dimensions[0] * 2 : dimensions[shapeChar == 'R' ? 1 : 0]));
        
        addObjectAtPosition(posX, posY, defaultMass, shapeChar, dimensions, color);
    }

    private void addObjectAtPosition(double x, double y, double mass, char shapeChar, 
                                   double[] dimensions, Color color) {
        try {
            // Initial velocity
            double velX = 0;
            double velY = 0;

            // Add object to physics engine
            PhysicsEngineJNI.addObject(worldPtr, nextId, mass, x, y, velX, velY, shapeChar, dimensions);
            
            // Store shape information for rendering
            objectShapes.put(nextId, new ShapeInfo(shapeChar, dimensions, color));
            
            nextId++;
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
        result.ifPresent(massStr -> {
            try {
                double mass = Double.parseDouble(massStr);
                
                // Show shape selection dialog
                List<String> choices = Arrays.asList("Rectangle", "Circle", "Square");
                ChoiceDialog<String> shapeDialog = new ChoiceDialog<>("Rectangle", choices);
                shapeDialog.setTitle("Select Shape");
                shapeDialog.setHeaderText("Choose object shape:");
                shapeDialog.setContentText("Shape:");

                Optional<String> shapeResult = shapeDialog.showAndWait();
                shapeResult.ifPresent(shape -> {
                    char shapeChar;
                    double[] dimensions;
                    Color color = Color.color(random.nextDouble(), random.nextDouble(), random.nextDouble());

                    switch (shape) {
                        case "Rectangle":
                            dimensions = new double[]{50.0, 30.0};
                            shapeChar = 'R';
                            break;
                        case "Circle":
                            dimensions = new double[]{20.0};
                            shapeChar = 'C';
                            break;
                        case "Square":
                            dimensions = new double[]{40.0};
                            shapeChar = 'S';
                            break;
                        default:
                            showError("Invalid shape selection");
                            return;
                    }

                    addObjectAtPosition(x, y, mass, shapeChar, dimensions, color);
                });
            } catch (NumberFormatException e) {
                showError("Invalid mass value");
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

    public void toggleVectorDisplay() {
        isShowingVectors = !isShowingVectors;
        render();
    }

    public void startSimulation() {
        isRunning = true;
    }

    public void pauseSimulation() {
        isRunning = false;
    }

    public void resetSimulation() {
        isRunning = false;
        clearCanvas();
        nextId = 1;
        objectShapes.clear();
        PhysicsEngineJNI.deletePhysicsWorld(worldPtr);
        worldPtr = PhysicsEngineJNI.createPhysicsWorld();
    }

    public boolean isRunning() {
        return isRunning;
    }

    public void cleanup() {
        isRunning = false;
        objectShapes.clear();
    }
}
