package java;
//package com.java;
import java.util.Scanner; 

public class PhysicsSimulation {
    // Load the native library containing C++ implementations
    static {
        System.loadLibrary("physics_native");
    }

    // Native method declarations for C++ functions
    private native void handleCollisions(long worldPtr);
    private native long createPhysicsWorld();
    private native void deletePhysicsWorld(long worldPtr);
    private native void addObject(long worldPtr, int id, double mass, double posX, double posY, 
                                double velX, double velY, char shape, double... dimensions);
    private native void configureForces(long worldPtr, int choice, double... params);
    private native void stepSimulation(long worldPtr, double deltaTime);
    private native void displayObjectInfo(long worldPtr, int objectId);

    private static Scanner scanner = new Scanner(System.in);
    private long worldPtr;
    private int nextId = 1;

    public PhysicsSimulation() {
        worldPtr = createPhysicsWorld();
    }

    private static <T extends Number> T getValidInput(String prompt, Class<T> type) {
        while (true) {
            try {
                System.out.print(prompt);
                String input = scanner.nextLine();
                if (type == Integer.class) {
                    return type.cast(Integer.parseInt(input));
                } else if (type == Double.class) {
                    return type.cast(Double.parseDouble(input));
                }
                throw new IllegalArgumentException("Unsupported type");
            } catch (NumberFormatException e) {
                System.out.println("Invalid input. Please try again.");
            }
        }
    }

    public void run() {
        boolean running = true;
        System.out.println("Welcome to the Enhanced Physics Simulation!");

        while (running) {
            System.out.println("\n=== Physics Simulation Menu ===");
            System.out.println("1. Add new object");
            System.out.println("2. Configure forces");
            System.out.println("3. Handle collisions");
            System.out.println("4. Step simulation");
            System.out.println("5. Exit");

            int choice = getValidInput("Enter your choice (1-5): ", Integer.class);

            switch (choice) {
                case 1:
                    handleAddObject();
                    break;
                case 2:
                    handleConfigureForces();
                    break;
                case 3:
                    handleCollisions(worldPtr);
                    break;
                case 4:
                    handleStepSimulation();
                    break;
                case 5:
                    running = false;
                    System.out.println("Thank you for using the Enhanced Physics Simulation!");
                    break;
                default:
                    System.out.println("Invalid choice. Please try again.");
            }
        }
    }

    private void handleAddObject() {
        double mass = getValidInput("Enter mass: ", Double.class);
        
        System.out.println("Enter position (x, y): ");
        double posX = getValidInput("X: ", Double.class);
        double posY = getValidInput("Y: ", Double.class);
        
        System.out.println("Enter velocity (x, y): ");
        double velX = getValidInput("X: ", Double.class);
        double velY = getValidInput("Y: ", Double.class);

        System.out.println("Choose shape of object");
        System.out.println("R - Rectangle");
        System.out.println("C - Circle");
        System.out.println("S - Square");
        
        char shape = scanner.nextLine().toUpperCase().charAt(0);
        double[] dimensions;
        
        switch (shape) {
            case 'R':
                System.out.println("Enter length and width: ");
                double length = getValidInput("Length: ", Double.class);
                double width = getValidInput("Width: ", Double.class);
                dimensions = new double[]{length, width};
                break;
            case 'C':
                double radius = getValidInput("Enter radius: ", Double.class);
                dimensions = new double[]{radius};
                break;
            case 'S':
                double sideLength = getValidInput("Enter side length: ", Double.class);
                dimensions = new double[]{sideLength};
                break;
            default:
                System.out.println("Invalid shape choice!");
                return;
        }

        addObject(worldPtr, nextId++, mass, posX, posY, velX, velY, shape, dimensions);
        System.out.println("Object created successfully!");
    }

    private void handleConfigureForces() {
        System.out.println("\n=== Configure Forces ===");
        System.out.println("1. Set Friction Coefficients (mu_s, mu_k)");
        System.out.println("2. Set Gravitational Acceleration");
        System.out.println("3. Back to Main Menu");

        int choice = getValidInput("Enter your choice (1-3): ", Integer.class);

        switch (choice) {
            case 1:
                double mu_s = getValidInput("Enter static friction coefficient (mu_s): ", Double.class);
                double mu_k = getValidInput("Enter kinetic friction coefficient (mu_k): ", Double.class);
                configureForces(worldPtr, choice, mu_s, mu_k);
                break;
            case 2:
                double g = getValidInput("Enter gravitational acceleration (g): ", Double.class);
                configureForces(worldPtr, choice, g);
                break;
        }
    }

    private void handleStepSimulation() {
        double deltaTime = getValidInput("Enter time step (in seconds): ", Double.class);
        stepSimulation(worldPtr, deltaTime);
        System.out.println("\nSimulation stepped forward by " + deltaTime + " seconds.");
    }

    public void cleanup() {
        deletePhysicsWorld(worldPtr);
    }

    public static void main(String[] args) {
        PhysicsSimulation simulation = new PhysicsSimulation();
        try {
            simulation.run();
        } finally {
            simulation.cleanup();
        }
    }
}

