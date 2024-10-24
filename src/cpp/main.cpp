#include "object.h"
#include "forces.h"
#include "physics_world.h"
#include "vector2d.h"
#include "collider.h"
#include <iostream>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Helper function to initialize ImGui
void InitImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

// Helper function to render ImGui window
void RenderImGui(PhysicsWorld& world) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Physics Simulation");
    ImGui::Text("Block Simulation");
    
    // Render information about the objects in the world
    for (const auto& object : world.getObjects()) {
        ImGui::Text("Object ID: %d", object->getID());
        ImGui::Text("Position: (%.2f, %.2f)", object->getPosition().x, object->getPosition().y);
        ImGui::Text("Velocity: (%.2f, %.2f)", object->getVelocity().x, object->getVelocity().y);
        ImGui::Separator();
    }
    
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
    // Initialize GLFW and create a window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Physics Simulation", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    InitImGui(window);  // Initialize ImGui

    // Create the PhysicsWorld
    PhysicsWorld world;
    
    // Create a block object (Rectangle)
    Rectangle block(1, 10.0f, Vector2D(0, 0), Vector2D(0, 0), 1.0f, 1.0f);
    
    // Add block to the world
    world.addObject(&block);

    // Apply forces to the block: 5N to the right and 6N to the left
    Vector2D forceRight(5, 0);  // Force to the right
    Vector2D forceLeft(-6, 0);  // Force to the left
    
    Forces::applyCustomForce(block, forceRight);  // Apply 5N force to the right
    Forces::applyCustomForce(block, forceLeft);   // Apply 6N force to the left

    // Main simulation loop
    while (!glfwWindowShouldClose(window)) {
        // Simulate one step
        world.step(0.01f);  // Small time step for the simulation

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render the ImGui window
        RenderImGui(world);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup ImGui and GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}

