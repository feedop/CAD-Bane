export module inputhandler;

import std;
import glm;

import <GLFW/glfw3.h>;
import <imgui/imgui/imgui.h>;

import camera;
import scene;
import raycaster;
import renderer;
import math;

/// <summary>
/// Handles user input (mouse, keyboard, and window events) for the application.
/// Integrates GLFW callbacks to interact with the Camera, Scene, Raycaster, and Renderer.
/// </summary>
export class InputHandler
{
public:
	/// <summary>
	/// Constructor for InputHandler, registering GLFW input callbacks.
	/// </summary>
	/// <param name="window">Pointer to the GLFW window instance.</param>
	/// <param name="camera">Reference to the Camera object for handling view adjustments.</param>
	/// <param name="scene">Reference to the Scene object for object selection and transformation.</param>
	/// <param name="raycaster">Reference to the Raycaster for updating the render process.</param>
	/// <param name="renderer">Reference to the Renderer for managing screen size changes.</param>
	InputHandler(GLFWwindow* window, Camera& camera, Scene& scene, Raycaster& raycaster, Renderer& renderer) :
		camera(camera), scene(scene), raycaster(raycaster), renderer(renderer)
	{
		glfwSetWindowUserPointer(window, this);

		glfwSetFramebufferSizeCallback(window, viewportCallback);
		glfwSetMouseButtonCallback(window, mouseCallback);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyboardCallback);
	}

	/// <summary>
	/// Callback function for window resizing, updating camera aspect ratio and renderer window size.
	/// </summary>
	/// <param name="window">GLFW window instance.</param>
	/// <param name="width">New width of the window.</param>
	/// <param name="height">New height of the window.</param>
	static void viewportCallback(GLFWwindow* window, int width, int height)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->camera.setAspect(static_cast<float>(width) / static_cast<float>(height));
		handler->renderer.setWindowSize(width, height);
	}

	/// <summary>
	/// Callback function for mouse button input. Handles object selection and camera movement initiation.
	/// </summary>
	/// <param name="window">GLFW window instance.</param>
	/// <param name="button">Mouse button pressed (GLFW_MOUSE_BUTTON_LEFT, etc.).</param>
	/// <param name="action">Action (GLFW_PRESS or GLFW_RELEASE).</param>
	/// <param name="mods">Modifier keys (Shift, Ctrl, etc.).</param>
	static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (ImGui::GetIO().WantCaptureMouse)
			return;

		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->handleMouseInput(window, button, action);
	}

	/// <summary>
	/// Callback function for scroll wheel input, used for camera zooming.
	/// </summary>
	/// <param name="window">GLFW window instance.</param>
	/// <param name="xoffset">Horizontal scroll offset (unused).</param>
	/// <param name="yoffset">Vertical scroll offset (determines zoom direction).</param>
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->camera.zoom(static_cast<float>(yoffset));
		handler->raycaster.enqueueUpdate();
	}

	/// <summary>
	/// Callback function for keyboard input, handling key presses and releases.
	/// </summary>
	/// <param name="window">GLFW window instance.</param>
	/// <param name="key">Key pressed (GLFW_KEY_A, GLFW_KEY_R, etc.).</param>
	/// <param name="scancode">System-specific scancode of the key.</param>
	/// <param name="action">Action (GLFW_PRESS, GLFW_RELEASE).</param>
	/// <param name="mods">Modifier keys (Shift, Ctrl, etc.).</param>
	static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_LEFT_SHIFT:
				handler->pressedKeys.SHIFT = true;
				break;
			case GLFW_KEY_LEFT_CONTROL:
				handler->pressedKeys.CTRL = true;
				break;
			case GLFW_KEY_LEFT_ALT:
				handler->pressedKeys.ALT = true;
				break;
			case GLFW_KEY_Z:
				handler->pressedKeys.Z = true;
				break;
			case GLFW_KEY_A:
				handler->scene.addToCurves();
				break;
			case GLFW_KEY_R:
				handler->scene.removeFromCurves();
				break;	
			case GLFW_KEY_DELETE:
			case GLFW_KEY_BACKSPACE:
				handler->scene.removeObjects();
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_LEFT_SHIFT:
				handler->pressedKeys.SHIFT = false;
				break;
			case GLFW_KEY_LEFT_CONTROL:
				handler->pressedKeys.CTRL = false;
				break;
			case GLFW_KEY_LEFT_ALT:
				handler->pressedKeys.ALT = false;
				break;
			case GLFW_KEY_Z:
				handler->pressedKeys.Z = false;
				break;
			}
		}
	}
	
	// <summary>
	/// Handles mouse button events, enabling camera and object manipulation.
	/// </summary>
	/// <param name="window">GLFW window instance.</param>
	/// <param name="button">Mouse button pressed.</param>
	/// <param name="action">Action (GLFW_PRESS or GLFW_RELEASE).</param>
	void handleMouseInput(GLFWwindow* window, int button, int action)
	{
		if(action == GLFW_PRESS)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_RIGHT:
				mouseMovingMode = true;
				glfwGetCursorPos(window, &lastX, &lastY);
				initialX = lastX;
				initialY = lastY;
				break;
			case GLFW_MOUSE_BUTTON_LEFT:
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				renderer.selectObjectFromScreen(x, y, pressedKeys.CTRL);
				break;
			}		
		}
		else if (action == GLFW_RELEASE)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_RIGHT:
				mouseMovingMode = false;
				break;
			}
		}
	}

	/// <summary>
	/// Processes mouse movement events to handle camera and object transformations.
	/// </summary>
	/// <param name="window">GLFW window instance.</param>
	void handleMouseMovement(GLFWwindow* window)
	{
		if (!mouseMovingMode)
			return;

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		double dx = x - lastX;
		double dy = y - lastY;

		if ((dx < math::eps && dx > -math::eps) && (dy < math::eps && dy > -math::eps))
			return;

		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		float xDiff = static_cast<float>(dx) / windowWidth * math::pi;
		float yDiff = static_cast<float>(dy) / windowHeight * math::pi;

		// Cursor movement
		if (pressedKeys.ALT)
		{
			scene.moveCursor(xDiff, yDiff);
		}
		// Object movement and rotation
		else if (pressedKeys.CTRL)
		{
			if (pressedKeys.SHIFT)
			{
				scene.moveObjects(xDiff, yDiff);
			}
			else if (pressedKeys.Z)
			{
				if (initialX < windowWidth / 2)
					xDiff *= -1;
				if (initialY < windowHeight / 2)
					yDiff *= -1;
				scene.scaleObjects(xDiff, yDiff);
			}
			else
			{
				scene.rotateObjects(xDiff, yDiff);
			}
		}
		// Camera movement and rotation
		else
		{
			if (pressedKeys.SHIFT)
			{
				camera.move(xDiff, yDiff);
			}
			else
			{
				camera.rotate(xDiff, yDiff);
			}
		}
			
		raycaster.enqueueUpdate();
		lastX = x;
		lastY = y;
	}

private:
	Camera& camera;
	Scene& scene;
	Raycaster& raycaster;
	Renderer& renderer;

	double lastX = 0;
	double lastY = 0;
	double initialX = 0;
	double initialY = 0;

	bool mouseMovingMode = false;

	struct PressedKeys
	{
		bool SHIFT = false;
		bool CTRL = false;
		bool ALT = false;
		bool Z = false;
	} pressedKeys;
};

