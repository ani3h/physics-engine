# physics-engine
The Physics Simulation Engine is designed to simulate the movement and interaction of objects under various forces such as gravity, friction, and collisions.
The purpose of this engine is to provide accurate and real-time simulation for applications like video games, physics research, or educational tools. 
The system will simulate rigid body dynamics, handle force application, detect collisions between objects, and provide realistic collision responses. 

The simulation will support: 
- Rigid body simulation with customizable objects and forces. 
- Basic force effects including gravity, friction, and user-defined forces. 
- Collision detection for axis-aligned bounding boxes (AABB) and circle colliders. 
- Real-time updates of objects in response to forces and collisions. 

## Project Directory Structure
```
├── docs/
│   └── Physics Simulation Engine (project documentation)
├── include/
│   └── jni.h (generated JNI header for C++ integration)
├── lib/
│   └── libPhysicsEngine.so (compiled shared library for the physics engine)
├── resources/
│   └── temp/ (resource files such as images, config files, etc.)
├── src/
│   ├── cpp/
│   │   ├── main.cpp (entry point for the C++ engine)
│   │   ├── object.cpp/.h (object properties and methods)
│   │   ├── forces.cpp/.h (force application properties and methods)
│   │   ├── physics_world.cpp/.h (core physics simulation logic)
│   │   └── collider.cpp/.h (collision detection and response logic)
│   ├── java/
│   │   ├── PhysicsSimulation.java (Java entry point for middleware)
│   │   ├── GUIController.java (JavaFX controller for handling UI interactions)
│   │   ├── SimulationApp.java (main entry point for JavaFX GUI)
│   │   └── jni/
│   │       └── PhysicsEngineJNI.java (Java JNI wrapper to call C++ methods)
├── tests/
│   ├── cpp/
│   │   ├── test_object.cpp (unit tests for object class)
│   │   ├── test_collider.cpp (unit tests for collision detection)
│   │   └── test_physics_world.cpp (unit tests for core simulation logic)
│   ├── java/
│   │   ├── test_jni_integration.java (integration tests for Java and C++ interaction)
│   │   └── test_gui_interaction.java (tests for JavaFX GUI interactions)
├── .gitignore
└── README.md
```

## Getting Started

To install the package, you can use pip with the URL of the GitHub repository.

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/your-username/physics-engine
   cd physics-engine
   ```
2. **Set up C++ Environment:**
   
   Navigate to the src/cpp/ directory:
   ```bash
   cd src/cpp
   ```
   Compile the C++ physics engine: 
   ```bash
   g++ -fPIC -shared -o ../../lib/libPhysicsEngine.so main.cpp object.cpp physics_world.cpp collider.cpp 
   ```
   This will generate the libPhysicsEngine.so shared library in the lib/ directory.
   
4. **Set Up Java Environment:**
   
   Navigate to the src/java/ directory:
   ```bash
   cd ../../src/java
   ```
   Compile the Java frontend and JNI integration:
   ```bash
   javac -d . PhysicsSimulation.java jni/PhysicsEngineJNI.java
   ```
   If the JNI header (jni.h) is not already generated, run:
   ```bash
   javah -jni jni.PhysicsEngineJNI
   ```
   This generates jni_PhysicsEngineJNI.h and should be included in your C++ code for JNI communication.
   
6. **Set Up JavaFX GUI:**
   
   Ensure the JavaFX SDK is downloaded and properly configured.
   Run the JavaFX application with the correct --module-path for the JavaFX SDK:
   ```bash
   java --module-path /path/to/javafx-sdk/lib --add-modules javafx.controls,javafx.fxml -cp . PhysicsSimulation
   ```

8. **Run the Simulation:**
   
   Ensure that the libPhysicsEngine.so is available in your library path:
   ```bash
   export LD_LIBRARY_PATH=../../lib:$LD_LIBRARY_PATH
   ```
   Execute the Java middleware to connect the console-based input or JavaFX GUI with the C++ physics engine:
   ```bash
   java PhysicsSimulation 
   ```
**Note:**
   Make sure you have the necessary tools and dependencies installed for both Java and JavaFX to run the respective environments.
