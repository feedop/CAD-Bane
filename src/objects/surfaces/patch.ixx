export module patch;

import std;

import <glad/glad.h>;

import <glm/vec3.hpp>;
import <glm/vec4.hpp>;
import <Serializer/Serializer.h>;

import colors;
import drawable;
import glutils;
import math;
import parametric;
import point;
import shader;

export class Patch : public Drawable, public Parametric
{
public:

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

	virtual ~Patch()
	{
		ScopedBindArray ba(transposedVAO);

		glBindBuffer(GL_ARRAY_BUFFER, transposedVBO);
		glDeleteBuffers(1, &transposedVBO);

		glDeleteVertexArrays(1, &transposedVAO);
	}

	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawArrays(GL_PATCHES, 0, positions.size());
	}

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

	void replacePoint(const Point* oldPoint, Point* newPoint)
	{
		auto it = std::find(points.begin(), points.end(), oldPoint);
		if (it != points.end())
			*it = newPoint;
	}

	inline const auto& getPoints() const
	{
		return points;
	}

	virtual glm::vec3 evaluate(float u, float v) const override
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
		return math::deCasteljau3(bernsteins[0], bernsteins[1], bernsteins[2], bernsteins[3], u);
	}

	virtual glm::vec3 derivativeU(float u, float v) const override
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
		return math::deCasteljau3Derivative(bernsteins[0], bernsteins[1], bernsteins[2], bernsteins[3], u);
	}

	virtual glm::vec3 derivativeV(float u, float v) const override
	{
		glm::vec3 bernsteins[4];
		for (int i = 0; i < 4; i++)
		{
			bernsteins[i] = math::deCasteljau3Derivative(
				points[i]->getPosition(),
				points[4 + i]->getPosition(),
				points[8 + i]->getPosition(),
				points[12 + i]->getPosition(),
				v
			);
		}
		return math::deCasteljau3(bernsteins[0], bernsteins[1], bernsteins[2], bernsteins[3], u);
	}

private:
	inline static const glm::vec4 polygonColor = colors::orange;

	unsigned int transposedVAO = 0, transposedVBO = 0;

	std::vector<Point*> points;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> positionsTransposed;
};