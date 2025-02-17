export module raycaster;

import std;

import <glad/glad.h>;

import camera;
import canvas;
import ellipsoid;
import cpuraycasting;
import gpuraycasting;
import math;
import raycastingop;
import raycastingstrategy;
import shader;

/// <summary>
/// A class that performs raycasting operations and manages rendering strategies.
/// </summary>
export class Raycaster
{
public:
	friend class GuiController;

	/// <summary>
	/// Constructs a Raycaster object with the given camera and ellipsoid.
	/// </summary>
	/// <param name="camera">The camera used for viewing the scene.</param>
	/// <param name="ellipsoid">The ellipsoid object used in the raycasting process.</param>
	Raycaster(const Camera& camera, const Ellipsoid& ellipsoid) : ellipsoid(ellipsoid), camera(camera)
	{}

	/// <summary>
	/// Enqueues an update operation for raycasting.
	/// This method clears the operation queue and repopulates it based on the selected adaptation level.
	/// </summary>
	void enqueueUpdate()
	{
		if (!enabled)
			return;

		operationQ = {};
		if (selectedAdaptationLevelOnly)
		{
			operationQ.emplace(maxTexWidth >> adaptationLevel, maxTexHeight >> adaptationLevel);
		}
		else
		{
			for (int i = adaptationLevel; i >= 0; i--)
			{
				int width = maxTexWidth >> i;
				int height = maxTexHeight >> i;
				operationQ.emplace(width, height);
			}
		}		
	}

	/// <summary>
	/// Draws the raycasted scene using the specified shader.
	/// </summary>
	/// <param name="shader">The shader to be used for rendering.</param>
	inline void draw(const Shader* shader)
	{
		if (!enabled)
			return;

		// Update if needed
		if (!operationQ.empty())
		{
			auto&& op = operationQ.front();
			operationQ.pop();
			op.execute(*strategy, lightM);
		}

		shader->use();
		glBindTexture(GL_TEXTURE_2D, strategy->getTexId());
		glViewport(0, 0, maxTexWidth, maxTexHeight);
		canvas.draw(shader);
	}

	/// <summary>
	/// Sets the texture size for raycasting and triggers an update.
	/// </summary>
	/// <param name="width">The desired width of the texture.</param>
	/// <param name="height">The desired height of the texture.</param>
	inline void setTextureSize(int width, int height)
	{
		maxTexWidth = width;
		maxTexHeight = height;
		enqueueUpdate();
	}

	/// <summary>
	/// Sets the rendering mode to either GPU or CPU and triggers an update.
	/// </summary>
	/// <param name="gpuRender">A boolean value indicating whether GPU rendering should be enabled.</param>
	inline void setMode(bool gpuRender)
	{
		if (gpuRender)
			strategy = gpuRaycasting.get();
		else
			strategy = cpuRaycasting.get();
		enqueueUpdate();
	}

private:
	bool enabled = false;

	const Canvas canvas;
	const Camera& camera;
	const Ellipsoid& ellipsoid;

	const std::unique_ptr<RaycastingStrategy> cpuRaycasting = std::make_unique<CpuRaycasting>(camera, ellipsoid);
	const std::unique_ptr<RaycastingStrategy> gpuRaycasting = std::make_unique<GpuRaycasting>(camera, ellipsoid, canvas);
	RaycastingStrategy* strategy = gpuRaycasting.get();

	std::queue<RaycastingOp> operationQ;

	int lightM = 4;
	int maxTexWidth = 0;
	int maxTexHeight = 0;
	int adaptationLevel = 0;
	bool selectedAdaptationLevelOnly = false;
};