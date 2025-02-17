export module application;

import std;

import <glad/glad.h>;
import <GLFW/glfw3.h>;

/// <summary>
/// The Application class initializes and manages the main GLFW window.
/// It sets up OpenGL context, handles errors, and ensures proper termination.
/// </summary>
export class Application
{
public:
    /// <summary>
    /// Constructor: Initializes GLFW, sets up OpenGL context, and creates a window.
    /// </summary>
    /// <param name="initialWidth">Initial width of the window.</param>
    /// <param name="initialHeight">Initial height of the window.</param>
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
        glfwSwapInterval(1);

        // Load GL and set the viewport to match window size
        gladLoadGL();
	}

    /// <summary>
    /// Destructor: Cleans up GLFW resources upon application exit.
    /// </summary>
    ~Application()
    {
        glfwTerminate();
    }

    /// <summary>
    /// Gets the GLFW window pointer.
    /// </summary>
    /// <returns>Pointer to the GLFW window instance.</returns>
    inline GLFWwindow* getWindowPtr()
    {
        return window;
    }

private:
    GLFWwindow* window = nullptr;

    /// <summary>
    /// GLFW error callback function. Prints errors to the console.
    /// </summary>
    /// <param name="error">Error code.</param>
    /// <param name="description">Error description.</param>
    static void errorCallback(int error, const char* description)
    {
        std::cerr << "Glfw Error " << error << ": " << description << "\n";
    }
};