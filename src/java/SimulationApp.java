package JAVA;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.scene.layout.VBox;
import javafx.scene.canvas.Canvas;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.layout.HBox;
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
        Button addObjectBtn = new Button("Add Object");
        Button startBtn = new Button("Start");
        Button pauseBtn = new Button("Pause");
        Button resetBtn = new Button("Reset");
        ComboBox<String> objectType = new ComboBox<>();
        objectType.getItems().addAll("Rectangle", "Circle", "Square");
        objectType.setValue("Rectangle");

        controls.getChildren().addAll(objectType, addObjectBtn, startBtn, pauseBtn, resetBtn);

        // Add components to root
        root.getChildren().addAll(canvas, controls);

        // Create the controller
        controller = new GUIController(canvas, simulation, worldPtr);

        // Set up button actions
        addObjectBtn.setOnAction(e -> controller.handleAddObject(objectType.getValue()));
        startBtn.setOnAction(e -> controller.startSimulation());
        pauseBtn.setOnAction(e -> controller.pauseSimulation());
        resetBtn.setOnAction(e -> controller.resetSimulation());

        // Create game loop
        gameLoop = new AnimationTimer() {
            private long lastUpdate = 0;
            
            @Override
            public void handle(long now) {
                if (lastUpdate == 0) {
                    lastUpdate = now;
                    return;
                }
                
                // Convert nanoseconds to seconds
                double deltaTime = (now - lastUpdate) * 1e-9;
                lastUpdate = now;
                
                // Update physics and redraw
                if (controller.isRunning()) {
                    controller.update(deltaTime);
                }
            }
        };
        
        // Start the game loop
        gameLoop.start();

        // Create and show the scene
        Scene scene = new Scene(root);
        primaryStage.setTitle("Physics Simulation");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    @Override
    public void stop() {
        // Clean up resources
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
