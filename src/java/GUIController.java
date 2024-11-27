package JAVA;

import javafx.scene.Scene;
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
    private static final double VECTOR_SCALE = 20.0;
    private static final int VECTOR_TEXT_OFFSET = 15;

    // Force control variables
    private static final double HORIZONTAL_FORCE = 500.0;
    private boolean isLeftKeyPressed = false;
    private boolean isRightKeyPressed = false;
    private Integer selectedObjectId = null;

    private static class ShapeInfo {
        char type;
        double[] dimensions;
        Color color;
        double lastStableX;
        double lastStableY;
    
        ShapeInfo(char type, double[] dimensions, Color color) {
            this.type = type;
            this.dimensions = dimensions;
            this.color = color;
            this.lastStableX = 0;
            this.lastStableY = 0;
        }
    }

    private void checkFinalStability() {
        try {
            // Add a small delay before checking final stability
            Thread.sleep(100);
            
            // Very small threshold for final stability check
            final double FINAL_THRESHOLD = VELOCITY_THRESHOLD / 2;
            
            // Counter for stable frames
            final int REQUIRED_STABLE_FRAMES = 3;
            int stableFrameCount = 0;
            
            // Check stability over multiple frames
            for (int frame = 0; frame < REQUIRED_STABLE_FRAMES; frame++) {
                boolean allObjectsStable = true;
                
                for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
                    ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, entry.getKey());
                    if (state == null) continue;
                    
                    // Check velocities and accelerations
                    if (Math.abs(state.getVelX()) > FINAL_THRESHOLD || 
                        Math.abs(state.getVelY()) > FINAL_THRESHOLD ||
                        Math.abs(state.getAccX()) > FINAL_THRESHOLD || 
                        Math.abs(state.getAccY()) > FINAL_THRESHOLD) {
                        allObjectsStable = false;
                        break;
                    }
                    
                    // Store initial positions for the first frame
                    if (frame == 0) {
                        entry.getValue().lastStableX = state.getPosX();
                        entry.getValue().lastStableY = state.getPosY();
                    } else {
                        // Check if position has changed significantly
                        double positionDelta = Math.sqrt(
                            Math.pow(state.getPosX() - entry.getValue().lastStableX, 2) +
                            Math.pow(state.getPosY() - entry.getValue().lastStableY, 2)
                        );
                        
                        if (positionDelta > FINAL_THRESHOLD) {
                            allObjectsStable = false;
                            break;
                        }
                    }
                }
                
                if (allObjectsStable) {
                    stableFrameCount++;
                } else {
                    // Reset stable frame count if any instability is detected
                    stableFrameCount = 0;
                    break;
                }
                
                // Small delay between stability checks
                Thread.sleep(16); // Approximately 60 FPS
            }
            
            // Update simulation state based on stability check
            isRunning = stableFrameCount < REQUIRED_STABLE_FRAMES;
            
            // If simulation is stopping, zero out small residual velocities
            if (!isRunning) {
                for (Integer id : objectShapes.keySet()) {
                    ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
                    if (state == null) continue;
                    
                    if (Math.abs(state.getVelX()) < FINAL_THRESHOLD && 
                        Math.abs(state.getVelY()) < FINAL_THRESHOLD) {
                        PhysicsEngineJNI.updateObjectState(worldPtr, id,
                            state.getPosX(), state.getPosY(),
                            0.0, 0.0); // Zero out velocities
                    }
                }
            }
            
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            isRunning = false;
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
            } else {
                handleObjectSelection(e.getX(), e.getY());
            }
        });

        clearCanvas();
    }

    public void setupKeyHandling(Scene scene) {
        scene.setOnKeyPressed(event -> {
            switch (event.getCode()) {
                case LEFT:
                    isLeftKeyPressed = true;
                    break;
                case RIGHT:
                    isRightKeyPressed = true;
                    break;
                default:
                    break;
            }
        });

        scene.setOnKeyReleased(event -> {
            switch (event.getCode()) {
                case LEFT:
                    isLeftKeyPressed = false;
                    break;
                case RIGHT:
                    isRightKeyPressed = false;
                    break;
                default:
                    break;
            }
        });
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
                // Apply any user-controlled forces
                applyHorizontalForces();
                
                // Step the physics simulation
                PhysicsEngineJNI.stepSimulation(worldPtr, deltaTime);
                
                // Handle object-object collisions first
                PhysicsEngineJNI.handleCollisions(worldPtr);
                
                // Then handle boundary collisions
                handleBoundaryCollisions();
                
                // Update the display
                render();
            }
        }
    }

    private void handleBoundaryCollisions() {
        // Define boundaries
        final double MARGIN = 5.0;
        double leftBound = MARGIN;
        double rightBound = canvas.getWidth() - MARGIN;
        double topBound = MARGIN;
        double bottomBound = canvas.getHeight() - MARGIN;
        
        boolean anyObjectActive = false;
        
        for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
            int id = entry.getKey();
            ShapeInfo shapeInfo = entry.getValue();
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
            
            if (state == null) continue;
            
            // Calculate object boundaries
            double objectLeft = state.getPosX();
            double objectRight = objectLeft;
            double objectTop = state.getPosY();
            double objectBottom = objectTop;
            
            switch (shapeInfo.type) {
                case 'R':
                    objectRight += shapeInfo.dimensions[0];
                    objectBottom += shapeInfo.dimensions[1];
                    break;
                case 'C':
                    objectRight += shapeInfo.dimensions[0] * 2;
                    objectBottom += shapeInfo.dimensions[0] * 2;
                    break;
                case 'S':
                    objectRight += shapeInfo.dimensions[0];
                    objectBottom += shapeInfo.dimensions[0];
                    break;
            }
            
            double velX = state.getVelX();
            double velY = state.getVelY();
            double newPosX = state.getPosX();
            double newPosY = state.getPosY();
            boolean collisionOccurred = false;
            
            // Handle boundary collisions
            if (objectBottom > bottomBound) {
                newPosY = bottomBound - (objectBottom - objectTop);
                if (velY > 0) {
                    velY = -velY * GROUND_RESTITUTION;
                    velX *= (1.0 - GROUND_FRICTION);
                    collisionOccurred = true;
                }
            }
            
            if (objectTop < topBound) {
                newPosY = topBound;
                if (velY < 0) {
                    velY = -velY * GROUND_RESTITUTION;
                    velX *= (1.0 - GROUND_FRICTION);
                    collisionOccurred = true;
                }
            }
            
            if (objectRight > rightBound) {
                newPosX = rightBound - (objectRight - objectLeft);
                if (velX > 0) {
                    velX = -velX * GROUND_RESTITUTION;
                    velY *= (1.0 - GROUND_FRICTION);
                    collisionOccurred = true;
                }
            }
            
            if (objectLeft < leftBound) {
                newPosX = leftBound;
                if (velX < 0) {
                    velX = -velX * GROUND_RESTITUTION;
                    velY *= (1.0 - GROUND_FRICTION);
                    collisionOccurred = true;
                }
            }
            
            // Check if object has significant motion
            boolean hasSignificantMotion = Math.abs(velX) > VELOCITY_THRESHOLD || 
                                         Math.abs(velY) > VELOCITY_THRESHOLD;
            
            if (hasSignificantMotion || collisionOccurred) {
                anyObjectActive = true;
                PhysicsEngineJNI.updateObjectState(worldPtr, id, newPosX, newPosY, velX, velY);
            }
        }
        
        // Update simulation state
        if (!anyObjectActive && isRunning) {
            checkFinalStability();
        }
    }

    private void applyHorizontalForces() {
        if (selectedObjectId != null && (isLeftKeyPressed || isRightKeyPressed)) {
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, selectedObjectId);
            if (state != null) {
                double force = 0;
                if (isLeftKeyPressed) force -= HORIZONTAL_FORCE;
                if (isRightKeyPressed) force += HORIZONTAL_FORCE;
                
                // Update velocity directly for immediate response
                double newVelX = state.getVelX() + (force / 1.0) * 0.016; // Assuming 60 FPS
                PhysicsEngineJNI.updateObjectState(worldPtr, selectedObjectId,
                    state.getPosX(), state.getPosY(),
                    newVelX, state.getVelY());
            }
        }
    }

    private void handleGroundCollisions() {
        // Handle block-on-block collisions
        PhysicsEngineJNI.handleCollisions(worldPtr);
    
        // Define boundaries (adding small margins)
        final double MARGIN = 5.0;
        double leftBound = MARGIN;
        double rightBound = canvas.getWidth() - MARGIN;
        double topBound = MARGIN;
        double bottomBound = canvas.getHeight() - MARGIN;
        
        boolean anyObjectActive = false;
        
        // Check all objects for boundary collisions
        for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
            int id = entry.getKey();
            ShapeInfo shapeInfo = entry.getValue();
            
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
            if (state == null) continue;
            
            // Calculate object boundaries based on shape
            double objectLeft = state.getPosX();
            double objectTop = state.getPosY();
            double objectRight, objectBottom;
            
            switch (shapeInfo.type) {
                case 'R':
                    objectRight = objectLeft + shapeInfo.dimensions[0];
                    objectBottom = objectTop + shapeInfo.dimensions[1];
                    break;
                case 'C':
                    objectRight = objectLeft + shapeInfo.dimensions[0] * 2;
                    objectBottom = objectTop + shapeInfo.dimensions[0] * 2;
                    break;
                case 'S':
                    objectRight = objectLeft + shapeInfo.dimensions[0];
                    objectBottom = objectTop + shapeInfo.dimensions[0];
                    break;
                default:
                    continue;
            }
            
            // Get current velocities
            double velX = state.getVelX();
            double velY = state.getVelY();
            double newPosX = state.getPosX();
            double newPosY = state.getPosY();
            boolean collisionOccurred = false;
            
            // Bottom boundary collision
            if (objectBottom > bottomBound) {
                newPosY = bottomBound - (objectBottom - objectTop);
                if (velY > 0) {
                    velY = -velY * GROUND_RESTITUTION;
                    velX *= (1.0 - GROUND_FRICTION);
                    collisionOccurred = true;
                }
            }
            
            // Top boundary collision
            if (objectTop < topBound) {
                newPosY = topBound;
                if (velY < 0) {
                    velY = -velY * GROUND_RESTITUTION;
                    velX *= (1.0 - GROUND_FRICTION);
                    collisionOccurred = true;
                }
            }
            
            // Right boundary collision
            if (objectRight > rightBound) {
                newPosX = rightBound - (objectRight - objectLeft);
                if (velX > 0) {
                    velX = -velX * GROUND_RESTITUTION;
                    velY *= (1.0 - GROUND_FRICTION);
                    collisionOccurred = true;
                }
            }
            
            // Left boundary collision
            if (objectLeft < leftBound) {
                newPosX = leftBound;
                if (velX < 0) {
                    velX = -velX * GROUND_RESTITUTION;
                    velY *= (1.0 - GROUND_FRICTION);
                    collisionOccurred = true;
                }
            }
            
            // Check if object has significant motion
            boolean hasSignificantMotion = Math.abs(velX) > VELOCITY_THRESHOLD || 
                                         Math.abs(velY) > VELOCITY_THRESHOLD;
            
            // Update simulation state
            if (hasSignificantMotion || collisionOccurred) {
                anyObjectActive = true;
            }
            
            // Update object state if position or velocity changed
            if (collisionOccurred) {
                PhysicsEngineJNI.updateObjectState(worldPtr, id, newPosX, newPosY, velX, velY);
            }
        }
        
        // Update simulation running state
        if (isRunning && !anyObjectActive) {
            try {
                Thread.sleep(100); // Small delay to ensure stability
                
                // Final check for motion
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

    private void handleObjectSelection(double clickX, double clickY) {
        for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
            int id = entry.getKey();
            ShapeInfo shapeInfo = entry.getValue();
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
            
            if (state == null) continue;
            
            double posX = state.getPosX();
            double posY = state.getPosY();
            double width = 0;
            double height = 0;
            
            switch (shapeInfo.type) {
                case 'R':
                    width = shapeInfo.dimensions[0];
                    height = shapeInfo.dimensions[1];
                    break;
                case 'C':
                    width = shapeInfo.dimensions[0] * 2;
                    height = shapeInfo.dimensions[0] * 2;
                    break;
                case 'S':
                    width = shapeInfo.dimensions[0];
                    height = shapeInfo.dimensions[0];
                    break;
            }
            
            if (clickX >= posX && clickX <= posX + width &&
                clickY >= posY && clickY <= posY + height) {
                selectedObjectId = id;
                return;
            }
        }
        selectedObjectId = null;
    }
    
    private void drawVelocityVector(double x, double y, double velX, double velY) {
        // Calculate velocity magnitude
        double velocityMagnitude = Math.sqrt(velX * velX + velY * velY);
        
        // Skip drawing if velocity is effectively zero
        if (velocityMagnitude < 0.01) return;
        
        // Normalize the velocity vector
        double normalizedVelX = velX / velocityMagnitude;
        double normalizedVelY = velY / velocityMagnitude;
        
        // Use a fixed length for the vector visualization
        final double FIXED_VECTOR_LENGTH = 40.0;
        
        // Calculate end point using normalized direction and fixed length
        double scaledEndX = x + normalizedVelX * FIXED_VECTOR_LENGTH;
        double scaledEndY = y + normalizedVelY * FIXED_VECTOR_LENGTH;
        
        // Draw velocity vector in black
        gc.setStroke(Color.BLACK);
        gc.setLineWidth(2);
        
        // Draw the main vector line
        gc.strokeLine(x, y, scaledEndX, scaledEndY);
        
        // Draw arrowhead
        double angle = Math.atan2(normalizedVelY, normalizedVelX);
        double arrowLength = 10;
        
        gc.strokeLine(scaledEndX, scaledEndY,
                     scaledEndX - arrowLength * Math.cos(angle - Math.PI/6),
                     scaledEndY - arrowLength * Math.sin(angle - Math.PI/6));
        gc.strokeLine(scaledEndX, scaledEndY,
                     scaledEndX - arrowLength * Math.cos(angle + Math.PI/6),
                     scaledEndY - arrowLength * Math.sin(angle + Math.PI/6));
        
        // Display velocity magnitude
        gc.setFill(Color.BLACK);
        gc.setFont(new Font("Arial", 12));
        String velocityText = String.format("%.2f m/s", velocityMagnitude);
        gc.fillText(velocityText, 
                   x + VECTOR_TEXT_OFFSET, 
                   y - VECTOR_TEXT_OFFSET);
    }

    private void render() {
        clearCanvas();
        
        // Draw boundary box
        gc.setStroke(Color.BLACK);
        gc.setLineWidth(2);
        final double MARGIN = 5.0;
        // Draw the boundary rectangle
        gc.strokeRect(MARGIN, MARGIN, 
                     canvas.getWidth() - 2*MARGIN, 
                     canvas.getHeight() - 2*MARGIN);
        
        for (Map.Entry<Integer, ShapeInfo> entry : objectShapes.entrySet()) {
            int id = entry.getKey();
            ShapeInfo shapeInfo = entry.getValue();
            
            ObjectState state = PhysicsEngineJNI.getObjectState(worldPtr, id);
            if (state == null) continue;
            
            double posX = state.getPosX();
            double posY = state.getPosY();
            
            // Highlight selected object
            if (selectedObjectId != null && selectedObjectId == id) {
                gc.setStroke(Color.BLUE);
                gc.setLineWidth(2);
                
                // Draw selection highlight
                switch (shapeInfo.type) {
                    case 'R':
                        gc.strokeRect(posX - 2, posY - 2, 
                                    shapeInfo.dimensions[0] + 4, 
                                    shapeInfo.dimensions[1] + 4);
                        break;
                    case 'C':
                        double diameter = shapeInfo.dimensions[0] * 2;
                        gc.strokeOval(posX - 2, posY - 2, 
                                    diameter + 4, diameter + 4);
                        break;
                    case 'S':
                        gc.strokeRect(posX - 2, posY - 2, 
                                    shapeInfo.dimensions[0] + 4, 
                                    shapeInfo.dimensions[0] + 4);
                        break;
                }
            }
            
            // Draw the object
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
                drawVelocityVector(centerX, centerY, state.getVelX(), state.getVelY());
            }
        }
        
        // Draw instructions if an object is selected
        if (selectedObjectId != null) {
            gc.setFill(Color.BLACK);
            gc.setFont(new Font("Arial", 14));
            gc.fillText("Use LEFT/RIGHT arrow keys to apply force", 10, 20);
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

        double posX = random.nextDouble() * (canvas.getWidth() - dimensions[0] - 10) + 5;
        double posY = random.nextDouble() * (canvas.getHeight() * 0.5);
        
        addObjectAtPosition(posX, posY, defaultMass, shapeChar, dimensions, color, false);
    }

    private void addObjectAtPosition(double x, double y, double mass, char shapeChar, 
                                   double[] dimensions, Color color, boolean placeOnGround) {
        try {
            double posY = y;
            if (placeOnGround) {
                // Calculate position to place object on ground
                double objectHeight = (shapeChar == 'C') ? dimensions[0] * 2 :
                                    (shapeChar == 'R') ? dimensions[1] : dimensions[0];
                posY = canvas.getHeight() - 5 - objectHeight;
            }

            // Initial velocity (reduced for ground-placed objects)
            double velX = placeOnGround ? 0 : random.nextDouble() * 2 - 1;
            double velY = 0;

            PhysicsEngineJNI.addObject(worldPtr, nextId, mass, x, posY, velX, velY, shapeChar, dimensions);
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
                    boolean placeOnGround = y > canvas.getHeight() * 0.8; // Place on ground if click is near bottom

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

                    addObjectAtPosition(x, y, mass, shapeChar, dimensions, color, placeOnGround);
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
        selectedObjectId = null;
        PhysicsEngineJNI.deletePhysicsWorld(worldPtr);
        worldPtr = PhysicsEngineJNI.createPhysicsWorld();
    }

    public boolean isRunning() {
        return isRunning;
    }

    public void cleanup() {
        isRunning = false;
        objectShapes.clear();
        selectedObjectId = null;
    }
}