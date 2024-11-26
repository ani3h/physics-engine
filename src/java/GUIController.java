package JAVA;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.control.Alert;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.ChoiceDialog;
import javafx.scene.text.Font;
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
    private Map<Integer, ShapeInfo> objectShapes;
    private boolean isShowingAcc = false;
    private static final double GROUND_RESTITUTION = 0.6;
    private static final float GROUND_FRICTION = 0.2f;
    private static final double VELOCITY_THRESHOLD = 0.1;
    private static final double VECTOR_SCALE = 20.0; // Scale factor for velocity vectors
    private static final int VECTOR_TEXT_OFFSET = 15; // Offset for velocity text

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
            synchronized(this) {
                PhysicsEngineJNI.stepSimulation(worldPtr, deltaTime);
                PhysicsEngineJNI.handleCollisions(worldPtr);
                handleGroundCollisions();
                render();
            }
        }
    }

    private void handleGroundCollisions() {
        double groundY = canvas.getHeight() - 5; // Ground level
        boolean anyObjectActive = false;
        
        // First pass: Check all objects and handle collisions
        for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
            int id = entry.getKey();
            ShapeInfo shapeInfo = entry.getValue();
            
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
            if (state == null) continue;
            
            // Calculate object boundaries
            double objectBottom;
            switch (shapeInfo.type) {
                case 'R': objectBottom = state.getPosY() + shapeInfo.dimensions[1]; break;
                case 'C': objectBottom = state.getPosY() + shapeInfo.dimensions[0] * 2; break;
                case 'S': objectBottom = state.getPosY() + shapeInfo.dimensions[0]; break;
                default: continue;
            }
            
            // Check if object is moving
            boolean hasSignificantMotion = Math.abs(state.getVelX()) > VELOCITY_THRESHOLD || 
                                         Math.abs(state.getVelY()) > VELOCITY_THRESHOLD;
            
            // Check if object is above ground
            boolean isAboveGround = objectBottom < groundY;
            
            // If object is either moving or not yet at rest position, simulation should continue
            if (hasSignificantMotion || isAboveGround) {
                anyObjectActive = true;
            }
            
            // Handle ground collision if needed
            if (objectBottom > groundY) {
                // Calculate correct Y position
                double correctedY;
                switch (shapeInfo.type) {
                    case 'R': correctedY = groundY - shapeInfo.dimensions[1]; break;
                    case 'C': correctedY = groundY - shapeInfo.dimensions[0] * 2; break;
                    case 'S': correctedY = groundY - shapeInfo.dimensions[0]; break;
                    default: continue;
                }
                
                // Apply collision response
                double velY = state.getVelY();
                double velX = state.getVelX();
                
                // Only apply bounce if moving downward
                if (velY > 0) {
                    velY = -velY * GROUND_RESTITUTION;
                    velX *= (1.0 - GROUND_FRICTION);
                    
                    // If bounce creates significant motion, keep simulation active
                    if (Math.abs(velY) > VELOCITY_THRESHOLD) {
                        anyObjectActive = true;
                    }
                }
                
                // Update object state
                PhysicsEngineJNI.updateObjectState(worldPtr, id, 
                                                 state.getPosX(), 
                                                 correctedY,
                                                 velX, 
                                                 velY);
            }
        }
        
        // Second pass: Update acceleration for all objects
        for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
            int id = entry.getKey();
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
            if (state == null) continue;
            
            // If any object has non-zero acceleration, keep simulation active
            if (Math.abs(state.getAccX()) > VELOCITY_THRESHOLD || 
                Math.abs(state.getAccY()) > VELOCITY_THRESHOLD) {
                anyObjectActive = true;
            }
        }
        
        // Only continue simulation if there are active objects
        if (isRunning && !anyObjectActive) {
            // Add a small delay before stopping to ensure all objects are truly at rest
            try {
                Thread.sleep(100); // Small delay to ensure stability
                
                // Double check if everything is really at rest
                boolean finalCheck = false;
                for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
                    ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, entry.getKey());
                    if (state == null) continue;
                    
                    if (Math.abs(state.getVelX()) > VELOCITY_THRESHOLD/2 || 
                        Math.abs(state.getVelY()) > VELOCITY_THRESHOLD/2 ||
                        Math.abs(state.getAccX()) > VELOCITY_THRESHOLD/2 || 
                        Math.abs(state.getAccY()) > VELOCITY_THRESHOLD/2) {
                        finalCheck = true;
                        break;
                    }
                }
                
                isRunning = finalCheck;
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        } else {
            isRunning = true;
        }
    }
    
    private void drawAccelerationVector(double x, double y, double accX, double accY) {
        // Calculate acceleration magnitude
        double accelerationMagnitude = Math.sqrt(accX * accX + accY * accY);
        
        // Use a different scale for acceleration since it's typically smaller than velocity
        // and might need more amplification to be visible
        final double ACCELERATION_SCALE = 100.0;  // Increased scale factor for acceleration
        
        // Draw acceleration vector in a different color to distinguish from velocity
        gc.setStroke(Color.BLACK);  // Changed to green to differentiate from velocity vectors
        gc.setLineWidth(2);
        
        // Scale the vector for better visualization
        double scaledEndX = x + accX * ACCELERATION_SCALE;
        double scaledEndY = y + accY * ACCELERATION_SCALE;
        
        // Draw the main vector line
        gc.strokeLine(x, y, scaledEndX, scaledEndY);
        
        // Draw arrowhead
        double angle = Math.atan2(accY, accX);
        double arrowLength = 10;
        
        gc.strokeLine(scaledEndX, scaledEndY,
                     scaledEndX - arrowLength * Math.cos(angle - Math.PI/6),
                     scaledEndY - arrowLength * Math.sin(angle - Math.PI/6));
        gc.strokeLine(scaledEndX, scaledEndY,
                     scaledEndX - arrowLength * Math.cos(angle + Math.PI/6),
                     scaledEndY - arrowLength * Math.sin(angle + Math.PI/6));
        
        // Display acceleration magnitude
        gc.setFill(Color.BLACK);  // Match the vector color
        gc.setFont(new Font("Arial", 12));
        String accelerationText = String.format("%.2f m/s²", accelerationMagnitude);
        gc.fillText(accelerationText, 
                   x + VECTOR_TEXT_OFFSET, 
                   y - VECTOR_TEXT_OFFSET);
    }

    private void render() {
        clearCanvas();
        
        // Draw ground line
        gc.setStroke(Color.BLACK);
        gc.setLineWidth(2);
        gc.strokeLine(0, canvas.getHeight() - 5, canvas.getWidth(), canvas.getHeight() - 5);
        
        for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
            int id = entry.getKey();
            ShapeInfo shapeInfo = entry.getValue();
            
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
            if (state == null) continue;
            
            double posX = state.getPosX();
            double posY = state.getPosY();
            gc.setFill(shapeInfo.color);
            
            // Calculate center point for vector drawing
            double centerX = posX;
            double centerY = posY;
            
            switch (shapeInfo.type) {
                case 'R':
                    gc.fillRect(posX, posY, shapeInfo.dimensions[0], shapeInfo.dimensions[1]);
                    centerX += shapeInfo.dimensions[0] / 2;
                    centerY += shapeInfo.dimensions[1] / 2;
                    break;
                case 'C':
                    double diameter = shapeInfo.dimensions[0] * 2;
                    gc.fillOval(posX, posY, diameter, diameter);
                    centerX += shapeInfo.dimensions[0];
                    centerY += shapeInfo.dimensions[0];
                    break;
                case 'S':
                    gc.fillRect(posX, posY, shapeInfo.dimensions[0], shapeInfo.dimensions[0]);
                    centerX += shapeInfo.dimensions[0] / 2;
                    centerY += shapeInfo.dimensions[0] / 2;
                    break;
            }
            
            // Draw velocity vector if enabled
            if (isShowingAcc) {
                drawAccelerationVector(centerX, centerY, state.getAccX(), state.getAccY());
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
                dimensions = new double[]{50.0, 30.0};
                shapeChar = 'R';
                break;
            case "CIRCLE":
                dimensions = new double[]{20.0};
                shapeChar = 'C';
                break;
            case "SQUARE":
                dimensions = new double[]{40.0};
                shapeChar = 'S';
                break;
            default:
                showError("Invalid shape type");
                return;
        }

        double posX = random.nextDouble() * (canvas.getWidth() - dimensions[0]);
        double posY = random.nextDouble() * (canvas.getHeight() * 0.5);
        
        addObjectAtPosition(posX, posY, defaultMass, shapeChar, dimensions, color);
    }

    private void addObjectAtPosition(double x, double y, double mass, char shapeChar, 
                                   double[] dimensions, Color color) {
        try {
            double velX = random.nextDouble() * 2 - 1;
            double velY = 0;

            PhysicsEngineJNI.addObject(worldPtr, nextId, mass, x, y, velX, velY, shapeChar, dimensions);
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
        isShowingAcc = !isShowingAcc;
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