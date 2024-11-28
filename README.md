# physics-engine
The Physics Simulation Engine is designed to simulate the movement and interaction of objects under various forces such as gravity, friction, and collisions.
The purpose of this engine is to provide accurate and real-time simulation for applications like video games, physics research, or educational tools. 
The system will simulate rigid body dynamics, handle force application, detect collisions between objects, and provide realistic collision responses. 

The simulation will support: 
- Rigid body simulation with customizable objects and forces. 
- Basic force effects including gravity, friction, and user-defined forces. 
- Collision detection for axis-aligned bounding boxes (AABB) and circle colliders. 
- Real-time updates of objects in response to forces and collisions.



![physics-sim](https://github.com/user-attachments/assets/54fd0a04-300d-4be1-bf76-26fc91270f49)



## Project Directory Structure
```
├── docs/
│   └── Physics Simulation Engine (project documentation)
├── include/
│   ├── colldier.h (collision detection and response logic)
│   ├── object.h (object properties and methods)
│   ├── forces.h (force application properties and methods)
│   ├── vector2d.h (2D vector operations)
│   ├── physics_native.h (Implementation of JNI methods)
│   ├── JAVA_jni_PhysicsEngineJNI
│   └── jni.h (generated JNI header for C++ integration)
├── lib/
│   └── libphysics_native.dylib (compiled shared library for the physics engine, generated on run)
├── resources/
│   └── javafx-sdk-23.0.1/ (javafx sdk)
│          └── lib/
│              └── ... (javafx library files)
├── src/
│   ├── module-info.java (defining properties of a module)
│   ├── cpp/
│   │   ├── collider.cpp (collision detection and response logic)
│   │   ├── forces.cpp (force application properties and methods)
│   │   ├── physics_native.cpp (Implementation of JNI methods)
│   │   └── object.cpp (object properties and methods)
│   ├── JAVA/
│   │   ├── PhysicsSimulation.java (Java entry point for middleware)
│   │   ├── GUIController.java (JavaFX controller for handling UI interactions)
│   │   ├── SimulationApp.java (main entry point for JavaFX GUI)
│   │   ├── ObjectState.java (hold physics state information)
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
├── build_and_run.sh (build and run script)
├── .gitignore
└── README.md
```

## Getting Started

To install the package, you can use pip with the URL of the GitHub repository.

To use the package, you can follow the steps below:

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/ani3h/physics-engine.git
   cd physics-engine
   ```
   
2. **Install JavaFX SDK:**
   
   Download the JavaFX SDK from the official website and extract it to the `resources/` directory, as seen in the directory structure.

3. **Make the build script executable(for mac):**
   
   Navigate to the root directory(Physics Engine):
   ```bash
   chmod +x build_and_run.sh
   ```
   
   This will compile the Java Code, generate JNI headers, compile the C++ code, create native library.
   
4. **Run the executabale:**
   
   Navigate to the src/java/ directory (for MAC):
   ```bash
   ./build_and_run.sh
   ```

   Run the batch file in Command Prompt (for WINDOWS):
   ```bash
   build_and_run.bat
   ```

   Runs the compiled Java application, and the JavaFX window pops up.
   
**Note:**
   Make sure you have the necessary tools and dependencies installed for both Java and JavaFX to run the respective environments.
