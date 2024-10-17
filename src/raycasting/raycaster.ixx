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

export class Raycaster
{
public:
	friend class GuiController;

	Raycaster(const Camera& camera, const Ellipsoid& ellipsoid) : ellipsoid(ellipsoid), camera(camera)
	{}

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

	inline void setTextureSize(int width, int height)
	{
		maxTexWidth = width;
		maxTexHeight = height;
		enqueueUpdate();
	}

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