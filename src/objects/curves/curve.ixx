export module curve;

import <glad/glad.h>;
import <format>;
import <initializer_list>;
import <numeric>;
import <set>;
import <vector>;

import <glm/vec3.hpp>;

import colors;
import glutils;
import point;
import pointowner;
import shape;

export class Curve : public PointOwner, public Shape
{
public:
	virtual ~Curve()
	{
		for (auto&& point : points)
		{
			point->detach(this);
		}
	}

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

	virtual void drawQuadratic(const Shader* shader) const
	{
		
	}

	virtual void drawLines(const Shader* shader) const
	{
		
	}

	virtual void drawPolygon(const Shader* shader) const
	{
		shader->setVector("color", polygonColor);
		shader->setMatrix("model", glm::mat4{1.0f});
		ScopedBindArray ba(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, positions.size());
	}

	virtual bool isInterpolating() const
	{
		return false;
	}

	void addPoints(Point* toAdd)
	{
		points.push_back(toAdd);
		toAdd->attach(this);
		scheduleToUpdate();
	}

	void addPoints(const std::vector<Point*>& toAdd)
	{
		for (auto&& point : toAdd)
		{
			points.push_back(point);
			point->attach(this);
		}
		scheduleToUpdate();
	}

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

	glm::vec3 getCenter()
	{
		return std::accumulate(positions.begin(), positions.end(), glm::vec3{ 0.0f, 0.0f, 0.0f }) * (1.0f / positions.size());
	}

protected:
	static constexpr int degree = 3;

	inline static const glm::vec4 polygonColor = colors::orange;

	std::vector<Point*> points;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> positionsQuadratic;
	std::vector<glm::vec3> positionsLine;

	Curve(const std::string& curveName, const std::vector<Point*>& points) : Shape(curveName), points(points)
	{
		attachPointsToCurve();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}

	virtual std::string getCurveName() const = 0;

	virtual void genBuffers()
	{
		// Empty default
	}

	virtual void fillPositions() = 0;

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

private:

	void attachPointsToCurve()
	{
		for (auto&& point : points)
		{
			point->attach(this);
		}
	}
};