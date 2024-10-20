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
│   └── Physics Simulation Engine (for project documentation)
├── include/
│   └── jni.h (generated JNI header for C++ integration)
├── lib/
│   └── libPhysicsEngine.so (compiled shared library for the physics engine)
├── resources/
│   └── temp (resource files such as images, config files, etc.)
├── src/
│   ├── cpp/
│   │   ├── main.cpp (entry point for the C++ engine)
│   │   ├── object.cpp/.h (object properties and methods)
│   │   ├── physics_world.cpp/.h (physics simulation logic)
│   │   └── collider.cpp/.h (collision detection logic)
│   ├── java/
│   │   ├── PhysicsSimulation.java (Java entry point)
│   │   └── jni/
│   │       └── PhysicsEngineJNI.java (Java JNI wrapper to call C++ methods)
│   └── nextjs/
│       ├── pages/
│       │   ├── index.js (main Next.js page for simulation control)
│       │   └── api/
│       │       └── simulation.js (API to communicate with the C++ engine via Java)
│       ├── components/
│       │   ├── ControlPanel.js (Component for user controls such as adding objects, forces, etc.)
│       │   └── SimulationCanvas.js (Component for rendering the simulation visualization)
│       └── public/
│           └── assets/ (Static files like icons, images)
├── tests/
│   ├── cpp/
│   │   ├── test_object.cpp (Tests for object class)
│   │   ├── test_collider.cpp (Tests for collision detection)
│   │   └── test_physics_world.cpp (Tests for core simulation logic)
│   ├── java/ (Integration tests for Java and C++ interaction)
│   └── nextjs/ (Tests for frontend functionality and API interactions)
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
2. **Set up C++ Environment**
   Navigate to the src/cpp/ directory:
   ```bash
   cd src/cpp
   ```
   Compile the C++ physics engine: 
   ```bash
   g++ -fPIC -shared -o ../../lib/libPhysicsEngine.so main.cpp object.cpp physics_world.cpp collider.cpp 
   ```
   This will generate the libPhysicsEngine.so shared library in the lib/ directory.
   
4. **Set Up Java Environment**
   Navigate to the src/java/ directory:
   ```bash
   cd ../../src/java
   ```
   Compile the Java frontend and JNI integration:
   ```bash
   javac -cp . -d . PhysicsSimulation.java jni/PhysicsEngineJNI.java
   ```
   If the JNI header (jni.h) is not already generated, run:
   ```bash
   javah -jni jni.PhysicsEngineJNI
   ```
   This generates jni_PhysicsEngineJNI.h and should be included in your C++ code for JNI communication.
   
6. **Set Up Next.js Frontend:**
   Navigate to the src/nextjs/ directory:
   ```bash
   cd ../../src/nextjs
   ```
   Install the required Node.js dependencies:
   ```bash
   npm install
   ```
   Start the Next.js development server:
   ```bash
   npm run dev
   ```
   This will launch the frontend on http://localhost:3000.

7. **Run the Simulation:**
   Ensure that the libPhysicsEngine.so is available in your library path:
   ```bash
   export LD_LIBRARY_PATH=../../lib:$LD_LIBRARY_PATH
   ```
   Execute the Java program, which will invoke the C++ physics engine via JNI:
   ```bash
   java PhysicsSimulation 
   ```
**Note:**
   Make sure you have the necessary tools and dependencies installed for both Java and Node.js to run the respective environments.
