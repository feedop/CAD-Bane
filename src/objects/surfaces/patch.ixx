export module patch;

import std;
import glm;

import <glad/glad.h>;

import <Serializer/Serializer.h>;

import colors;
import drawable;
import glutils;
import math;
import parametric;
import point;
import shader;

/// <summary>
/// Class representing a single Bezier patch, which is a part of a C0 surface.
/// </summary>
export class Patch : public Drawable, public Parametric
{
public:
	/// <summary>
	/// Constructor to create a patch with a list of points.
	/// </summary>
	/// <param name="pointList">A list of points that define the patch's geometry.</param>
	Patch(const std::vector<Point*>& pointList)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glGenVertexArrays(1, &transposedVAO);
		glGenBuffers(1, &transposedVBO);

		points = pointList;
		positions.resize(points.size());
		positionsTransposed.resize(points.size());

		update();
	}

	/// <summary>
	/// Destructor to clean up resources used by the Patch.
	/// </summary>
	virtual ~Patch()
	{
		ScopedBindArray ba(transposedVAO);

		glBindBuffer(GL_ARRAY_BUFFER, transposedVBO);
		glDeleteBuffers(1, &transposedVBO);

		glDeleteVertexArrays(1, &transposedVAO);
	}

	// <summary>
	/// Draw the patch using the specified shader.
	/// </summary>
	/// <param name="shader">The shader to be used for rendering the patch.</param>
	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawArrays(GL_PATCHES, 0, positions.size());
	}

	/// <summary>
	/// Update the patch's geometry by refreshing the positions of its points.
	/// </summary>
	void update()
	{
		for (int i = 0; i < points.size(); i++)
		{
			positions[i] = points[i]->getPosition();
		}
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				positionsTransposed[j * 4 + i] = points[i * 4 + j]->getPosition();
			}
		}
		{
			ScopedBindArray ba(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}
		{
			ScopedBindArray tba(transposedVAO);
			glBindBuffer(GL_ARRAY_BUFFER, transposedVBO);
			glBufferData(GL_ARRAY_BUFFER, positionsTransposed.size() * sizeof(glm::vec3), &positionsTransposed[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}
	}

	// <summary>
	/// Draw the polygonal representation of the patch using the specified shader.
	/// </summary>
	/// <param name="shader">The shader to be used for rendering the polygonal representation.</param>
	void drawPolygon(const Shader* shader) const
	{
		shader->setVector("color", polygonColor);
		shader->setMatrix("model", glm::mat4{ 1.0f });
		{
			ScopedBindArray ba(VAO);
			// Vertical
			for (int i = 0; i < 4; i++)
			{
				glDrawArrays(GL_LINE_STRIP, i * 4, 4);
			}
		}
		{
			ScopedBindArray tba(transposedVAO);
			// Horizontal
			for (int i = 0; i < 4; i++)
			{
				glDrawArrays(GL_LINE_STRIP, i * 4, 4);
			}
		}	
	}

	/// <summary>
	/// Serialize the patch to a Bezier surface in the MG1 system.
	/// </summary>
	/// <param name="surface">The Bezier surface to which the patch will be added.</param>
	/// <param name="allPoints">All points available for the surface.</param>
	void addToMGSurface(MG1::BezierSurfaceC0& surface, const std::vector<std::unique_ptr<Point>>& allPoints) const
	{
		auto find = [&](const Point* point)
		{
			for (int i = 0; i < allPoints.size(); i++)
			{
				if (allPoints[i].get() == point)
				{
					return i;
				}
			}
			return 0;
		};

		MG1::BezierPatchC0 bp;
		bp.samples.x = 4;
		bp.samples.y = 4;
		for (auto&& point : points)
		{
			bp.controlPoints.emplace_back(find(point) + 1);
		}
		surface.patches.push_back(bp);
	}

	/// <summary>
	/// Replace a point in the patch with a new point.
	/// </summary>
	/// <param name="oldPoint">The point to be replaced.</param>
	/// <param name="newPoint">The point to replace the old one.</param>
	void replacePoint(const Point* oldPoint, Point* newPoint)
	{
		auto it = std::find(points.begin(), points.end(), oldPoint);
		if (it != points.end())
			*it = newPoint;
	}

	/// <summary>
	/// Get the list of points that define the patch.
	/// </summary>
	/// <returns>A constant reference to the list of points.</returns>
	inline const auto& getPoints() const
	{
		return points;
	}

	/// <summary>
	/// Evaluates the surface at the given parameters (u, v) and with an optional tool radius.
	/// </summary>
	/// <param name="u">U parameter for the surface evaluation.</param>
	/// <param name="v">V parameter for the surface evaluation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D point corresponding to the (u, v) parameter on the surface.</returns>
	virtual glm::vec3 evaluate(float u, float v, float toolRadius) const override
	{
		glm::vec3 bernsteins[4];
		for (int i = 0; i < 4; i++)
		{
			bernsteins[i] = math::deCasteljau3(
				points[i]->getPosition(),
				points[4 + i]->getPosition(),
				points[8 + i]->getPosition(),
				points[12 + i]->getPosition(),
				v
			);
		}
		auto ret = math::deCasteljau3(bernsteins[0], bernsteins[1], bernsteins[2], bernsteins[3], u);

		if (toolRadius != 0.0f)
		{
			auto PU = derivativeU(u, v);
			auto PV = derivativeV(u, v);
			auto np = glm::cross(PU, PV);

			ret += glm::normalize(np) * toolRadius;
		}
		return ret;
	}

	/// <summary>
	/// Calculates the derivative of the surface in the U direction at the given parameters (u, v).
	/// </summary>
	/// <param name="u">U parameter for the derivative calculation.</param>
	/// <param name="v">V parameter for the derivative calculation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D vector representing the derivative in the U direction.</returns>
	virtual glm::vec3 derivativeU(float u, float v, float toolRadius = 0.0f) const override
	{
		if (u > 1.0f - math::derivativeH)
			return derivativeU(u - math::derivativeH, v, toolRadius);
		return (evaluate(u + math::derivativeH, v, toolRadius) - evaluate(u, v, toolRadius)) / math::derivativeH;

		// Analytical derivative
		/*glm::vec3 bernsteins[4];
		for (int i = 0; i < 4; i++)
		{
			bernsteins[i] = math::deCasteljau3(
				points[i]->getPosition(),
				points[4 + i]->getPosition(),
				points[8 + i]->getPosition(),
				points[12 + i]->getPosition(),
				v
			);
		}
		return math::deCasteljau3Derivative(bernsteins[0], bernsteins[1], bernsteins[2], bernsteins[3], u);*/
	}

	/// <summary>
	/// Calculates the derivative of the surface in the V direction at the given parameters (u, v).
	/// </summary>
	/// <param name="u">U parameter for the derivative calculation.</param>
	/// <param name="v">V parameter for the derivative calculation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D vector representing the derivative in the V direction.</returns>
	virtual glm::vec3 derivativeV(float u, float v, float toolRadius = 0.0f) const override
	{
		if (v > 1.0f - math::derivativeH)
			return derivativeV(u, v - math::derivativeH, toolRadius);
		return (evaluate(u, v + math::derivativeH, toolRadius) - evaluate(u, v, toolRadius)) / math::derivativeH;
	}

private:
	inline static const glm::vec4 polygonColor = colors::orange;

	unsigned int transposedVAO = 0, transposedVBO = 0;

	std::vector<Point*> points;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> positionsTransposed;
};