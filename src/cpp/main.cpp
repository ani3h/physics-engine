#include "object.h"
#include "forces.h"
#include "physics_world.h"
#include "vector2d.h"
#include "collider.h"
#include <iostream>
#include <memory>

// ImGui includes
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

class PhysicsSimulation {
private:
    GLFWwindow* window = nullptr;
    PhysicsWorld world;
    const char* glsl_version = "#version 130";

    bool initializeGLFW() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        // Create window with graphics context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        // Add this line to ensure proper OpenGL context
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(800, 600, "Physics Simulation", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync
        return true;
    }

    bool initializeImGui() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
            std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
            return false;
        }

        if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
            std::cerr << "Failed to initialize ImGui OpenGL3 backend" << std::endl;
            return false;
        }

        return true;
    }

    void renderImGui() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create a window
        ImGui::Begin("Physics Simulation", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // Display basic information
        ImGui::Text("Simulation Controls");
        ImGui::Separator();

        // Display object information
        for (const auto& object : world.getObjects()) {
            Vector2D pos = object->getPosition();
            Vector2D vel = object->getVelocity();

            ImGui::Text("Object %d", object->getID());
            ImGui::Text("Position: (%.2f, %.2f)", pos.x, pos.y);
            ImGui::Text("Velocity: (%.2f, %.2f)", vel.x, vel.y);
            ImGui::Separator();
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Get the draw data and render it
        ImDrawData* draw_data = ImGui::GetDrawData();
        if (draw_data) {
            ImGui_ImplOpenGL3_RenderDrawData(draw_data);
        }
    }

public:
    ~PhysicsSimulation() {
        cleanup();
    }

    void cleanup() {
        if (ImGui::GetCurrentContext() != nullptr) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        if (window != nullptr) {
            glfwDestroyWindow(window);
            window = nullptr;
        }

        glfwTerminate();
    }

    bool initialize() {
        if (!initializeGLFW()) {
            cleanup();
            return false;
        }

        if (!initializeImGui()) {
            cleanup();
            return false;
        }

        return true;
    }

    void setupScene() {
        // Create a block
        auto block = std::make_unique<Rectangle>(1, 10.0f, Vector2D(0, 0), Vector2D(0, 0), 1.0f, 1.0f);
        world.addObject(block.release());

        // Apply initial forces
        if (!world.getObjects().empty()) {
            Forces::applyCustomForce(*world.getObjects()[0], Vector2D(5, 0));   // Right force
            Forces::applyCustomForce(*world.getObjects()[0], Vector2D(-6, 0));  // Left force
        }
    }

    void run() {
        while (window != nullptr && !glfwWindowShouldClose(window)) {
            // Poll and handle events
            glfwPollEvents();

            // Update physics
            world.step(0.01f);

            // Render ImGui
            renderImGui();

            // Swap buffers
            glfwSwapBuffers(window);
        }
    }
};

int main() {
    PhysicsSimulation simulation;

    // Initialize the simulation (GLFW, ImGui, etc.)
    if (!simulation.initialize()) {
        return -1;  // Exit if initialization failed
    }

    // Setup the initial scene
    simulation.setupScene();

    // Run the simulation loop
    simulation.run();

    // Cleanup resources after the simulation loop exits
    simulation.cleanup();

    return 0;
}
