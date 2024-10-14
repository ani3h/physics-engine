# physics-engine
The Physics Simulation Engine is designed to simulate the movement and interaction of objects under various forces such as gravity, friction, and collisions.
The purpose of this engine is to provide accurate and real-time simulation for applications like video games, physics research, or educational tools. 
The system will simulate rigid body dynamics, handle force application, detect collisions between objects, and provide realistic collision responses. 

## Getting Started

To install the package, you can use pip with the URL of the GitHub repository.

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/your-username/physics-engine
   ```
2. **Set up C++ Environment**
   Compile the C++ physics engine: 
   ```bash
   g++ -o physics_engine main.cpp object.cpp physics_world.cpp -shared -o libPhysicsEngine.so 
   ```
3. **Set Up Java Environment**
   Compile the Java frontend and ensure JNI integration:
   ```bash
   javac -cp . -d . PhysicsSimulation.java
   ```
4. **Run the Simulation**
   Execute the Java program, which calls the C++ engine:
   ```bash
   java PhysicsSimulation 
   ```
   
