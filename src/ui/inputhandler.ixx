export module inputhandler;

import <iostream>;
import <glm/gtc/constants.hpp>;
import <GLFW/glfw3.h>;

import camera;
import raycaster;
import renderer;
import math;

export class InputHandler
{
public:
	InputHandler(GLFWwindow* window, Camera& camera, Raycaster& raycaster, Renderer& renderer) :
		camera(camera), raycaster(raycaster), renderer(renderer)
	{
		glfwSetWindowUserPointer(window, this);

		glfwSetFramebufferSizeCallback(window, viewportCallback);
		glfwSetMouseButtonCallback(window, mouseCallback);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyboardCallback);
	}

	static void viewportCallback(GLFWwindow* window, int width, int height)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->camera.setAspect(static_cast<float>(width) / static_cast<float>(height));
		handler->renderer.setWindowSize(width, height);
	}

	static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->handleMouseInput(window, button, action);
	}

	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->camera.zoom(static_cast<float>(yoffset));
		handler->raycaster.enqueueUpdate();
	}
	static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_LEFT_SHIFT:
				handler->pressedKeys.SHIFT = true;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_LEFT_SHIFT:
				handler->pressedKeys.SHIFT = false;
			}
		}
	}

	void handleMouseInput(GLFWwindow* window, int button, int action)
	{
		if (button != GLFW_MOUSE_BUTTON_LEFT)
			return;

		if(action == GLFW_PRESS)
		{	
			cameraMovingMode = true;
			glfwGetCursorPos(window, &lastX, &lastY);
		}
		else if (action == GLFW_RELEASE)
		{
			cameraMovingMode = false;
		}
	}

	void handleMouseMovement(GLFWwindow* window)
	{
		if (!cameraMovingMode)
			return;

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		double dx = x - lastX;
		double dy = y - lastY;

		if ((dx < math::eps && dx > -math::eps) && (dy < math::eps && dy > -math::eps))
			return;

		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		float xDiff = dx / windowWidth * math::pi;
		float yDiff = dy / windowHeight * math::pi;

		if (pressedKeys.SHIFT)
		{
			camera.move(xDiff, yDiff);
		}
		else
		{
			camera.rotate(xDiff, yDiff);
		}
		raycaster.enqueueUpdate();
		lastX = x;
		lastY = y;
	}

private:
	Camera& camera;
	Raycaster& raycaster;
	Renderer& renderer;

	double lastX = 0;
	double lastY = 0;

	bool cameraMovingMode = false;

	struct PressedKeys
	{
		bool SHIFT = false;
	} pressedKeys;
};

