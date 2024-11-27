package JAVA;

import javafx.application.Application;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.scene.layout.VBox;
import javafx.scene.layout.HBox;
import javafx.scene.canvas.Canvas;
import javafx.scene.control.*;
import javafx.geometry.Insets;
import javafx.animation.AnimationTimer;
import JAVA.jni.PhysicsEngineJNI;

public class SimulationApp extends Application {
    private GUIController controller;
    private PhysicsSimulation simulation;
    private long worldPtr;
    private AnimationTimer gameLoop;

    @Override
    public void start(Stage primaryStage) {
        // Initialize the physics simulation
        simulation = new PhysicsSimulation();
        worldPtr = PhysicsEngineJNI.createPhysicsWorld();

        // Create main layout
        VBox root = new VBox(10);
        root.setPadding(new Insets(10));

        // Create canvas for rendering
        Canvas canvas = new Canvas(800, 600);
        
        // Create control buttons
        HBox controls = new HBox(10);
        
        // Object controls
        ComboBox<String> objectType = new ComboBox<>();
        objectType.getItems().addAll("Rectangle", "Circle", "Square");
        objectType.setValue("Rectangle");
        Button addObjectBtn = new Button("Add Object");
        
        // Simulation controls
        Button startBtn = new Button("Start");
        Button pauseBtn = new Button("Pause");
        Button resetBtn = new Button("Reset");
        
        // Physics controls
        Button configureBtn = new Button("Configure Physics");
        ToggleButton showVectorsBtn = new ToggleButton("Show Vectors");
        
        // Add frictionless mode toggle
        ToggleButton frictionlessBtn = new ToggleButton("Frictionless Mode");
        
        controls.getChildren().addAll(
            objectType, addObjectBtn,
            new Separator(),
            startBtn, pauseBtn, resetBtn,
            new Separator(),
            configureBtn, showVectorsBtn,
            new Separator(),
            frictionlessBtn
        );

        // Add components to root
        root.getChildren().addAll(canvas, controls);

        // Create scene
        Scene scene = new Scene(root);

        // Create the controller
        controller = new GUIController(canvas, simulation, worldPtr);

        // Set up key handling for the scene
        controller.setupKeyHandling(scene);

        // Set up button actions with focus handling
        addObjectBtn.setOnAction(e -> {
            controller.handleAddObject(objectType.getValue());
            scene.getRoot().requestFocus();
        });
        
        startBtn.setOnAction(e -> {
            controller.startSimulation();
            scene.getRoot().requestFocus();
        });
        
        pauseBtn.setOnAction(e -> {
            controller.pauseSimulation();
            scene.getRoot().requestFocus();
        });
        
        resetBtn.setOnAction(e -> {
            controller.resetSimulation();
            scene.getRoot().requestFocus();
        });
        
        showVectorsBtn.setOnAction(e -> {
            controller.toggleVectorDisplay();
            scene.getRoot().requestFocus();
        });

        frictionlessBtn.setOnAction(e -> {
            controller.toggleFrictionless();
            scene.getRoot().requestFocus();
        });

        configureBtn.setOnAction(e -> {
            showConfigurationDialog();
            scene.getRoot().requestFocus();
        });

        // Create game loop
        gameLoop = new AnimationTimer() {
            private long lastUpdate = 0;
            
            @Override
            public void handle(long now) {
                if (lastUpdate == 0) {
                    lastUpdate = now;
                    return;
                }
                
                double deltaTime = (now - lastUpdate) * 1e-9; // Convert nanoseconds to seconds
                lastUpdate = now;
                
                if (controller.isRunning()) {
                    controller.update(deltaTime);
                }
            }
        };
        
        gameLoop.start();

        // Create and show the scene
        primaryStage.setTitle("Physics Simulation");
        primaryStage.setScene(scene);
        primaryStage.show();

        // Request focus for the scene to enable key events
        scene.getRoot().requestFocus();
    }

    private void showConfigurationDialog() {
        Dialog<Void> dialog = new Dialog<>();
        dialog.setTitle("Physics Configuration");
        dialog.setHeaderText("Configure Physics Parameters");

        // Create the content
        VBox content = new VBox(10);
        content.setPadding(new Insets(10));

        // Gravity control
        Label gravityLabel = new Label("Gravity (m/s²):");
        Slider gravitySlider = new Slider(0, 20, 9.81);
        gravitySlider.setShowTickLabels(true);
        gravitySlider.setShowTickMarks(true);

        // Friction controls
        Label frictionLabel = new Label("Friction Coefficients:");
        
        Label staticLabel = new Label("Static Friction:");
        Slider staticSlider = new Slider(0, 1, 0.5);
        staticSlider.setShowTickLabels(true);
        
        Label kineticLabel = new Label("Kinetic Friction:");
        Slider kineticSlider = new Slider(0, 1, 0.3);
        kineticSlider.setShowTickLabels(true);

        // Disable friction controls if in frictionless mode
        boolean isFrictionless = controller.isFrictionless();
        staticSlider.setDisable(isFrictionless);
        kineticSlider.setDisable(isFrictionless);
        
        // Add a note about frictionless mode if active
        if (isFrictionless) {
            Label frictionlessNote = new Label("Friction controls disabled - Frictionless mode is active");
            frictionlessNote.setStyle("-fx-text-fill: red;");
            content.getChildren().add(frictionlessNote);
        }

        content.getChildren().addAll(
            gravityLabel, gravitySlider,
            frictionLabel,
            staticLabel, staticSlider,
            kineticLabel, kineticSlider
        );

        // Add apply button
        ButtonType applyButtonType = new ButtonType("Apply", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(applyButtonType, ButtonType.CANCEL);

        // Set the content
        dialog.getDialogPane().setContent(content);

        // Handle the apply action
        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == applyButtonType) {
                // Configure gravity
                double[] gravityParams = {gravitySlider.getValue()};
                PhysicsEngineJNI.configureForces(worldPtr, 2, gravityParams);

                // Configure friction (only if not in frictionless mode)
                if (!controller.isFrictionless()) {
                    double[] frictionParams = {staticSlider.getValue(), kineticSlider.getValue()};
                    PhysicsEngineJNI.configureForces(worldPtr, 1, frictionParams);
                }
            }
            return null;
        });

        dialog.showAndWait();
    }

    @Override
    public void stop() {
        if (gameLoop != null) {
            gameLoop.stop();
        }
        if (controller != null) {
            controller.cleanup();
        }
        PhysicsEngineJNI.deletePhysicsWorld(worldPtr);
    }

    public static void main(String[] args) {
        launch(args);
    }
}