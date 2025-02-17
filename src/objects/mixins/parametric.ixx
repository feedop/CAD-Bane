export module parametric;

import glm;

import <glad/glad.h>;

import shader;

export
{
	class IntersectionCurve;
	class Parametric;
	class Scene;
	void removeSurfaceFromCurve(IntersectionCurve* curve, Parametric* surface);
	unsigned int getTrimmingTextureFromCurve(IntersectionCurve* curve, const Parametric* surface);
}

/// <summary>
/// Enum representing different trim modes for trimming operations.
/// </summary>
export enum class TrimMode : int
{
	/// <summary>
	/// Shows the entire object without trimming.
	/// </summary>
	Show,

	/// <summary>
	/// Hides the entire object.
	/// </summary>
	Hide,

	/// <summary>
	/// Removes white space from the parametric view during the trim operation.
	/// </summary>
	RemoveWhite,

	/// <summary>
	/// Removes black space from the parametric view during the trim operation.
	/// </summary>
	RemoveBlack
};

/// <summary>
/// Base class representing a parametric surface, with methods for evaluating points, derivatives, and handling intersections.
/// </summary>
export class Parametric
{
public:
	virtual ~Parametric()
	{
		if (intersectionCurve != nullptr)
			removeSurfaceFromCurve(intersectionCurve, this);
	}


	/// <summary>
	/// Evaluates the surface at the given parameters (u, v) and with an optional tool radius.
	/// </summary>
	/// <param name="u">U parameter for the surface evaluation.</param>
	/// <param name="v">V parameter for the surface evaluation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D point corresponding to the (u, v) parameter on the surface.</returns>
	virtual glm::vec3 evaluate(float u, float v, float toolRadius) const = 0;

	/// <summary>
	/// Calculates the derivative of the surface in the U direction at the given parameters (u, v).
	/// </summary>
	/// <param name="u">U parameter for the derivative calculation.</param>
	/// <param name="v">V parameter for the derivative calculation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D vector representing the derivative in the U direction.</returns>
	virtual glm::vec3 derivativeU(float u, float v, float toolRadius) const = 0;

	/// <summary>
	/// Calculates the derivative of the surface in the V direction at the given parameters (u, v).
	/// </summary>
	/// <param name="u">U parameter for the derivative calculation.</param>
	/// <param name="v">V parameter for the derivative calculation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D vector representing the derivative in the V direction.</returns>
	virtual glm::vec3 derivativeV(float u, float v, float toolRadius) const = 0;

	/// <summary>
	/// Returns the range of the parametric surface (default is 1.0f).
	/// </summary>
	/// <returns>The range of the surface.</returns>
	virtual constexpr float getRange() const
	{
		return 1.0f;
	}

	/// <summary>
	/// Adds an intersection curve to the parametric surface.
	/// </summary>
	/// <param name="curve">The intersection curve to be added.</param>
	void addIntersection(IntersectionCurve* curve)
	{
		intersectionCurve = curve;
	}

	/// <summary>
	/// Removes an intersection curve from the parametric surface.
	/// </summary>
	/// <param name="curve">The intersection curve to be removed.</param>
	void removeIntersection(IntersectionCurve* curve)
	{
		if (curve == nullptr || curve == intersectionCurve)
			intersectionCurve = nullptr;

		trimMode = TrimMode::Show;
	}

	/// <summary>
	/// Checks whether the parametric surface has an intersection.
	/// </summary>
	/// <returns>True if there is an intersection, false otherwise.</returns>
	inline bool hasIntersection() const
	{
		return intersectionCurve != nullptr;
	}

protected:
	/// <summary>
    /// Uploads the trim texture for this surface, if applicable, and sets the appropriate shader settings.
    /// </summary>
    /// <param name="shader">The shader to set the trim texture for.</param>
	void uploadTrimTexture(const Shader* shader) const
	{
		shader->setInt("trimMode", static_cast<int>(trimMode));
		if (hasIntersection())
			glBindTexture(GL_TEXTURE_2D, getTrimmingTextureFromCurve(intersectionCurve, this));
	}

private:
	friend void renderContextMenu(auto object, Scene* scene);
	IntersectionCurve* intersectionCurve;
	TrimMode trimMode = TrimMode::Show;
};