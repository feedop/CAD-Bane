export module surface;

import std;
import glm;

import <glad/glad.h>;

import <Serializer/Serializer.h>;

import parametric;
import pointrenderer;
import shape;
import shader;
import virtualpoint;

/// <summary>
/// A class representing a parametric surface that can be defined by a set of points and parameters.
/// This class is derived from <see cref="Shape"/> and <see cref="Parametric"/>. It allows for the 
/// manipulation of surface properties such as density, size, and translation, as well as rendering and adding 
/// the surface to a scene. It is an abstract base class that requires implementations for drawing, adding to 
/// a scene, and evaluating the surface points.
/// </summary>
export class Surface : public Shape, public Parametric
{
public:
	/// <summary>
	/// Gets the density along the X axis.
	/// </summary>
	/// <returns>The density along the X axis.</returns>
	inline float getDensityX() const
	{
		return densityX;
	}

	/// <summary>
	/// Sets the density along the X axis.
	/// </summary>
	/// <param name="newDensityX">The new density value for the X axis.</param>
	inline void setDensityX(float newDensityX)
	{
		densityX = newDensityX;
	}

	/// <summary>
	/// Gets the density along the Z axis.
	/// </summary>
	/// <returns>The density along the Z axis.</returns>
	inline float getDensityZ() const
	{
		return densityZ;
	}

	/// <summary>
	/// Sets the density along the Z axis.
	/// </summary>
	/// <param name="newDensityZ">The new density value for the Z axis.</param>
	inline void setDensityZ(float newDensityZ)
	{
		densityZ = newDensityZ;
	}

	/// <summary>
	/// Translates the surface by a given vector in 3D space, applying the translation to each point in the surface.
	/// </summary>
	/// <param name="positionChange">The translation vector to apply to the surface's points.</param>
	void translate(const glm::vec3& positionChange)
	{
		for (auto&& point : points)
		{
			point->translate(positionChange);
		}
	}

	// <summary>
	/// Draws the surface polygon using the provided shader.
	/// This method is abstract and must be implemented in derived classes.
	/// </summary>
	/// <param name="shader">The shader to use for rendering the surface polygon.</param>
	virtual void drawPolygon(const Shader* shader) const = 0;

	/// <summary>
	/// Serializes the surface to the provided MG1 scene, including all points from the surface.
	/// This method is abstract and must be implemented in derived classes.
	/// </summary>
	/// <param name="mgscene">The MG1 scene to add the surface to.</param>
	/// <param name="allPoints">The collection of all points in the scene.</param>
	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<std::unique_ptr<Point>>& allPoints) const = 0;

	/// <summary>
	/// Gets the list of points that define the surface.
	/// </summary>
	/// <returns>A reference to the list of points.</returns>
	inline const std::vector<Point*>& getPoints() const
	{
		return points;
	}

	/// <summary>
	/// Checks if the surface represents a cylinder.
	/// </summary>
	/// <returns>True if the surface is a cylinder, false otherwise.</returns>
	inline bool isCylinder() const
	{
		return cylinder;
	}

	/// <summary>
	/// Sets the size along the X axis for the surface.
	/// </summary>
	/// <param name="newSizeX">The new size value along the X axis.</param>
	inline void setSizeX(int newSizeX)
	{
		sizeX = newSizeX;
	}

	/// <summary>
	/// Gets the size along the Z axis for the surface.
	/// </summary>
	/// <returns>The size value along the Z axis.</returns>
	inline int getSizeZ() const
	{
		return sizeZ;
	}	

	/// <summary>
	/// Returns the preferred shader for rendering the surface.
	/// This method is abstract and must be implemented in derived classes.
	/// </summary>
	/// <returns>The preferred shader to use for rendering the surface.</returns>
	virtual Shader* getPreferredShader() const = 0;

	/// <summary>
	/// Evaluates the surface at the given parameters (u, v) and with an optional tool radius.
	/// </summary>
	/// <param name="u">U parameter for the surface evaluation.</param>
	/// <param name="v">V parameter for the surface evaluation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D point corresponding to the (u, v) parameter on the surface.</returns>
	virtual glm::vec3 evaluate(float u, float v, float toolRadius) const override
	{
		return { 0, 0, 0 };
	}

	/// <summary>
	/// Calculates the derivative of the surface in the U direction at the given parameters (u, v).
	/// </summary>
	/// <param name="u">U parameter for the derivative calculation.</param>
	/// <param name="v">V parameter for the derivative calculation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D vector representing the derivative in the U direction.</returns>
	virtual glm::vec3 derivativeU(float u, float v, float toolRadius) const override
	{
		return { 0, 0, 1 };
	}

	/// <summary>
	/// Calculates the derivative of the surface in the V direction at the given parameters (u, v).
	/// </summary>
	/// <param name="u">U parameter for the derivative calculation.</param>
	/// <param name="v">V parameter for the derivative calculation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D vector representing the derivative in the V direction.</returns>
	virtual glm::vec3 derivativeV(float u, float v, float toolRadius) const override
	{
		return { 1, 0, 0 };
	}

protected:
	static constexpr float patchSizeX = 1.0f;
	static constexpr float patchSizeZ = 1.0f;

	float densityX = 4.0f;
	float densityZ = 4.0f;

	int sizeX, sizeZ;
	bool cylinder;

	/// <summary>
	/// Constructor to initialize a surface object with specific parameters.
	/// </summary>
	/// <param name="surfaceName">The name of the surface.</param>
	/// <param name="sizeX">The size along the X axis.</param>
	/// <param name="sizeZ">The size along the Z axis.</param>
	/// <param name="cylinder">Whether the surface is a cylinder (default: false).</param>
	Surface(const std::string& surfaceName, int sizeX, int sizeZ, bool cylinder = false) : Shape(surfaceName), sizeX(sizeX), sizeZ(sizeZ), cylinder(cylinder)
	{
		
	}

	virtual std::string getSurfaceName() const = 0;

private:
	friend class GuiController;
};