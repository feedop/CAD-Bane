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
import objectrepository;
import raycaster;
import renderer;

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

    ObjectRepository repository;

    // Create a graphics controller
    Renderer renderer(cfg::initialWidth, cfg::initialHeight, camera, repository, raycaster);

    // Setup input handling
    InputHandler inputHandler(window, camera, raycaster, renderer);

    // Create a GUI controller
    GuiController guiController(window, repository, raycaster, renderer, ellipsoid);    

    // MAIN LOOP HERE - dictated by glfw
    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
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