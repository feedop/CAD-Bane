export module curve;

import std;
import glm;

import <glad/glad.h>;
import <Serializer/Serializer.h>;

import colors;
import glutils;
import point;
import pointowner;
import shape;

/// <summary>
/// Represents a curve that is defined by a series of points. This class is derived from <see cref="PointOwner"/> 
/// and <see cref="Shape"/> to provide functionality for managing points, drawing, and updating the curve.
/// </summary>
export class Curve : public PointOwner, public Shape
{
public:
	/// <summary>
	/// Draws the curve using a shader. The curve is rendered as a tessellated patch.
	/// </summary>
	/// <param name="shader">The shader used for rendering the curve.</param>
	virtual void draw(const Shader* shader) const override
	{
		if (positions.size() == 0)
			return;

		ScopedBindArray ba(VAO);
		ScopedLineWidth lw(2.0f);
		setColor(shader);
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawArrays(GL_PATCHES, 0, positions.size());
	}

	// <summary>
	/// Draws the curve's quadratic segment.
	/// </summary>
	/// <param name="shader">The shader used for rendering the curve.</param>
	virtual void drawQuadratic(const Shader* shader) const
	{
		// Empty default
	}

	// <summary>
	/// Draws the curve's line segment.
	/// </summary>
	/// <param name="shader">The shader used for rendering the curve.</param>
	virtual void drawLines(const Shader* shader) const
	{
		// Empty default
	}

	/// <summary>
	/// Draws the curve as a polygon.
	/// </summary>
	/// <param name="shader">The shader used for rendering the polygon representation of the curve.</param>
	virtual void drawPolygon(const Shader* shader) const
	{
		shader->setVector("color", polygonColor);
		shader->setMatrix("model", glm::mat4{1.0f});
		ScopedBindArray ba(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, positions.size());
	}

	// <summary>
	/// Determines if the curve is interpolating.
	/// </summary>
	/// <returns>Returns false by default. Override in subclasses to implement interpolation logic.</returns>
	virtual bool isInterpolating() const
	{
		return false;
	}

	/// <summary>
	/// Adds a point to the curve.
	/// </summary>
	/// <param name="toAdd">The point to add to the curve.</param>
	void addPoints(Point* toAdd)
	{
		points.push_back(toAdd);
		toAdd->attach(this);
		scheduleToUpdate();
	}
	
	/// <summary>
	/// Adds multiple points to the curve.
	/// </summary>
	/// <param name="toAdd">A vector of points to add to the curve.</param>
	void addPoints(const std::vector<Point*>& toAdd)
	{
		for (auto&& point : toAdd)
		{
			points.push_back(point);
			point->attach(this);
		}
		scheduleToUpdate();
	}

	/// <summary>
	/// Removes selected points from the curve.
	/// </summary>
	void removePoints()
	{
		for (auto&& point : points)
		{
			if (point->isSelected)
				point->detach(this);
		}
		std::erase_if(points, [](auto&& point) { return point->isSelected; });
		scheduleToUpdate();
	}

	/// <summary>
    /// Gets the center of the curve by averaging the positions of its points.
    /// </summary>
    /// <returns>The center of the curve as a <see cref="glm::vec3"/>.</returns>
	glm::vec3 getCenter()
	{
		return std::accumulate(positions.begin(), positions.end(), glm::vec3{ 0.0f, 0.0f, 0.0f }) * (1.0f / positions.size());
	}

	// <summary>
	/// Gets the points that define the curve.
	/// </summary>
	/// <returns>A reference to the vector of points.</returns>
	inline const std::vector<Point*>& getPoints() const
	{
		return points;
	}

	/// <summary>
	/// Gets the positions of the points in the curve.
	/// </summary>
	/// <returns>A reference to the vector of positions.</returns>
	inline const std::vector<glm::vec3>& getPositions() const
	{
		return positions;
	}

	/// <summary>
	/// Serializes the curve to the specified MG1 scene.
	/// </summary>
	/// <param name="mgscene">The MG1 scene to add the curve to.</param>
	/// <param name="indices">A vector of indices that define the curve.</param>
	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<unsigned int>& indices) const = 0;

protected:
	/// <summary>
	/// The degree of the curve. Default is 3 for cubic curves.
	/// </summary>
	static constexpr int degree = 3;

	inline static const glm::vec4 polygonColor = colors::orange;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> positionsQuadratic;
	std::vector<glm::vec3> positionsLine;

	// <summary>
	/// Constructor that initializes the curve with a name and a list of points.
	/// </summary>
	/// <param name="curveName">The name of the curve.</param>
	/// <param name="points">A vector of points that define the curve.</param>
	Curve(const std::string& curveName, const std::vector<Point*>& points) : Shape(curveName, points)
	{
		attachPoints();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}

	virtual std::string getCurveName() const = 0;

	// <summary>
	/// Generates the buffers required for the curve.
	/// </summary>
	virtual void genBuffers()
	{
		// Empty default
	}

	/// <summary>
	/// Fills the positions vector with the curve's positions.
	/// </summary>
	virtual void fillPositions() = 0;

	/// <summary>
	/// Uploads the curve's positions to the GPU.
	/// </summary>
	virtual void uploadPositions()
	{
		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (positions.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}
		else
		{
			glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
		}
	}
};