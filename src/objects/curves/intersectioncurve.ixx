export module intersectioncurve;

import std;
import glm;

import <glad/glad.h>;

import canvas;
import colors;
import glutils;
import interpolatingspline;
import parametric;
import parametricviewcanvas;
import point;
import shader;

/// <summary>
/// Represents the intersection curve between two parametric surfaces. 
/// The curve is defined by the intersection of two parametric surfaces, and it is treated as an interpolating spline.
/// </summary>
export class IntersectionCurve : public InterpolatingSpline
{
public:
	/// <summary>
	/// Initializes an intersection curve with two parametric surfaces and their associated positions and parameters.
	/// </summary>
	/// <param name="surface1">The first parametric surface involved in the intersection.</param>
	/// <param name="surface2">The second parametric surface involved in the intersection.</param>
	/// <param name="positions">The positions of the intersection curve in 3D space.</param>
	/// <param name="params">The parameters used for mapping the surfaces to their parametric space.</param>
	/// <param name="shader">The shader used for creating the parametric view.</param>
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

	/// <summary>
	/// Destructor that cleans up any allocated resources, such as textures and surface references.
	/// </summary>
	virtual ~IntersectionCurve()
	{
		if (surface1 != nullptr)
			surface1->removeIntersection(this);

		if (surface2 != nullptr)
			surface2->removeIntersection(this);

		glDeleteTextures(1, &surface1TrimmingTexture);
		glDeleteTextures(1, &surface2TrimmingTexture);
	}

	/// <summary>
	/// Updates the intersection curve, including filling positions, uploading data, and recalculating coefficients.
	/// </summary>
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
		// Not serializable
	}

	/// <summary>
	/// Removes a surface from the intersection curve's list of associated surfaces.
	/// </summary>
	/// <param name="surface">The surface to be removed from the intersection curve.</param>
	void removeSurface(Parametric* surface)
	{
		if (surface == surface1)
			surface1 = nullptr;
		if (surface == surface2)
			surface1 = nullptr;
	}

	/// <summary>
	/// Gets the parametric view canvas associated with one of the surfaces.
	/// </summary>
	/// <param name="firstSurface">If true, returns the canvas for the first surface; otherwise, returns the canvas for the second surface.</param>
	/// <returns>A shared pointer to the canvas used for parametric view of the surface.</returns>
	std::shared_ptr<Canvas>& getParametricViewCanvas(bool firstSurface)
	{
		return firstSurface ? surface1ParametricViewCanvas : surface2ParametricViewCanvas;
	}

	/// <summary>
	/// Gets the trimming texture associated with a given surface.
	/// </summary>
	/// <param name="caller">The surface to get the trimming texture for.</param>
	/// <returns>The trimming texture ID for the surface.</returns>
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

	/// <summary>
	/// Fills the positions of the curve - empty for the intersection curve.
	/// </summary>
	virtual void fillPositions() override
	{

	}
};

/// <summary>
/// Removes a surface from the intersection curve's list of associated surfaces.
/// </summary>
/// <param name="curve">The intersection curve from which the surface will be removed.</param>
/// <param name="surface">The surface to be removed.</param>
export void removeSurfaceFromCurve(IntersectionCurve* curve, Parametric* surface)
{
	curve->removeSurface(surface);
}

/// <summary>
/// Retrieves the trimming texture ID associated with a given surface on an intersection curve.
/// </summary>
/// <param name="curve">The intersection curve.</param>
/// <param name="surface">The surface for which to retrieve the trimming texture.</param>
/// <returns>The trimming texture ID for the surface.</returns>
export unsigned int getTrimmingTextureFromCurve(IntersectionCurve* curve, const Parametric* surface)
{
	return curve->getTrimmingTexture(surface);
}