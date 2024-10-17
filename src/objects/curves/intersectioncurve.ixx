export module intersectioncurve;

import std;

import <glad/glad.h>;
import <glm/vec3.hpp>;

import canvas;
import colors;
import glutils;
import interpolatingspline;
import parametric;
import parametricviewcanvas;
import point;
import shader;

export class IntersectionCurve : public InterpolatingSpline
{
public:
	IntersectionCurve(Parametric* surface1, Parametric* surface2, const std::vector<glm::vec3>& positions, const std::vector<glm::vec4> params, const Shader* shader) :
		InterpolatingSpline(getCurveName()), surface1(surface1), surface2(surface2)
	{
		this->positions = positions;
		
		std::vector<glm::vec3> params1;
		std::transform(params.begin(), params.end(), std::back_inserter(params1), [](auto&& pos) { return glm::vec3{2.0f * pos.x - 1.0f, 2.0f * pos.y - 1.0, 0.0f }; });
		surface1ParametricViewCanvas = std::make_shared<ParametricViewCanvas>(params1, shader, surface1TrimmingTexture);

		std::vector<glm::vec3> params2;
		std::transform(params.begin(), params.end(), std::back_inserter(params2), [](auto&& pos) { return glm::vec3{ 2.0f * pos.z - 1.0, 2.0f * pos.w - 1.0, 0.0f }; });
		surface2ParametricViewCanvas = std::make_shared<ParametricViewCanvas>(params2, shader, surface2TrimmingTexture);
	}

	virtual ~IntersectionCurve()
	{
		if (surface1 != nullptr)
			surface1->removeIntersection(this);

		if (surface2 != nullptr)
			surface2->removeIntersection(this);

		glDeleteTextures(1, &surface1TrimmingTexture);
		glDeleteTextures(1, &surface2TrimmingTexture);
	}

	virtual void update() override
	{
		if (!scheduledToUpdate)
			return;

		fillPositions();
		uploadPositions();
		calculateCoefficients();

		scheduledToUpdate = false;
	}

	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<unsigned int>& indices) const override
	{
		
	}

	void removeSurface(Parametric* surface)
	{
		if (surface == surface1)
			surface1 = nullptr;
		if (surface == surface2)
			surface1 = nullptr;
	}

	std::shared_ptr<Canvas>& getParametricViewCanvas(bool firstSurface)
	{
		return firstSurface ? surface1ParametricViewCanvas : surface2ParametricViewCanvas;
	}

	unsigned int getTrimmingTexture(const Parametric* caller) const
	{
		return caller == surface1 ? surface1TrimmingTexture : surface2TrimmingTexture;
	}

private:
	inline static unsigned int instanceCount = 0;

	Parametric* surface1;
	Parametric* surface2;

	std::shared_ptr<Canvas> surface1ParametricViewCanvas;
	std::shared_ptr<Canvas> surface2ParametricViewCanvas;
	unsigned int surface1TrimmingTexture = 0;
	unsigned int surface2TrimmingTexture = 0;

	virtual std::string getCurveName() const override
	{
		return std::format("{} {}", "Intersection curve", instanceCount++);
	}

	virtual void fillPositions() override
	{

	}
};

export void removeSurfaceFromCurve(IntersectionCurve* curve, Parametric* surface)
{
	curve->removeSurface(surface);
}

export unsigned int getTrimmingTextureFromCurve(IntersectionCurve* curve, const Parametric* surface)
{
	return curve->getTrimmingTexture(surface);
}