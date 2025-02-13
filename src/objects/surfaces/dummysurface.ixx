export module dummysurface;

import std;
import parametric;

export class DummySurface : public Parametric
{
public:
	virtual glm::vec3 evaluate(float u, float v, float toolRadius) const override
	{
		float x = 15.0f * u - 7.5f;
		float y = 15.0f * v - 7.5f;
		return { x, y, 1.5f + toolRadius };
	}

	virtual glm::vec3 derivativeU(float u, float v, float toolRadius) const override
	{
		return { 15.0f, 0.0f, 0.0f };
	}

	virtual glm::vec3 derivativeV(float u, float v, float toolRadius) const override
	{
		return { 0.0f, 15.0f, 0.0f };
	}
};