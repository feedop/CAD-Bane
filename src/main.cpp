#include <Serializer/Models/Point.h>

import <glad/glad.h>;
import <iostream>;
import <sstream>;
import <GLFW/glfw3.h>;

import application;
import camera;
import config;
import ellipsoid;
import gui;
import inputhandler;
import scene;
import pointrenderer;
import raycaster;
import renderer;

import c2surface;

#ifdef _WIN32
// Select an NVIDIA GPU if possible
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif


int main()
{
    Application application(cfg::initialWidth, cfg::initialHeight);
    GLFWwindow* window = application.getWindowPtr();

    Camera camera(0.0f, 0.0f, 5.0f);

    // Raycasting
    Ellipsoid ellipsoid{ 1.0f, 1.0f, 1.0f };
    Raycaster raycaster(camera, ellipsoid);

    PointRenderer pointRenderer;

    Scene scene(camera, pointRenderer);

    // Create a graphics controller
    Renderer renderer(cfg::initialWidth, cfg::initialHeight, camera, pointRenderer, scene, raycaster);

    // Setup input handling
    InputHandler inputHandler(window, camera, scene, raycaster, renderer);

    // Create a GUI controller
    GuiController guiController(window, camera, scene, raycaster, renderer, ellipsoid);

    // Initial objects
    scene.addSurface<C2Surface>(6, 1.0f);
    MG1::Point p;

    // MAIN LOOP HERE - dictated by glfw
    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        // Update curves if necessary
        scene.updateObjects();
        
        // OpenGL render
        renderer.draw();

        // GUI rendering
        guiController.draw();

        glfwSwapBuffers(window);

        // Show FPS in the title bar
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        if (delta >= 1.0)
        {
            double fps = double(frameCount) / delta;
            std::stringstream ss;
            ss << "CAD Bane" << " " << " [" << fps << " FPS]";

            glfwSetWindowTitle(window, ss.str().c_str());
            lastTime = currentTime;
            frameCount = 0;
        }
        else
        {
            frameCount++;
        }

        // Handle user input
        glfwPollEvents();
        inputHandler.handleMouseMovement(window);
    }

    return 0;
}