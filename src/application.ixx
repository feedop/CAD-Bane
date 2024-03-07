export module application;

import <glad/glad.h>;
import <iostream>;

import <GLFW/glfw3.h>;

export class Application
{
public:
	Application(int initialWidth, int initialHeight)
	{
        glfwSetErrorCallback(errorCallback);
        if (!glfwInit())
        {
            glfwTerminate();
            exit(1);
        }    

        // Initial settings
        const char* glsl_version = "#version 420";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create a windowed mode window and its OpenGL context
        window = glfwCreateWindow(initialWidth, initialHeight, "CAD Bane", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            exit(1);
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);

        // Set fps lock
        glfwSwapInterval(0);

        // Load GL and set the viewport to match window size
        gladLoadGL();
	}

    ~Application()
    {
        glfwTerminate();
    }

    inline GLFWwindow* getWindowPtr()
    {
        return window;
    }

private:
    GLFWwindow* window = nullptr;

    static void errorCallback(int error, const char* description)
    {
        std::cerr << "Glfw Error " << error << ": " << description << "\n";
    }
};