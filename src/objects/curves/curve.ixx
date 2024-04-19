export module curve;

import <glad/glad.h>;
import <format>;
import <initializer_list>;
import <numeric>;
import <set>;
import <vector>;

import <glm/vec3.hpp>;

import colors;
import drawable;
import glutils;
import point;
import selectable;

export class Curve : public Drawable, public Selectable
{
public:

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

	void drawQuadratic(const Shader* shader) const
	{
		if (positionsQuadratic.size() == 0)
			return;

		ScopedBindArray ba(quadraticVAO);
		setColor(shader);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawArrays(GL_PATCHES, 0, positionsQuadratic.size());
	}

	void drawLines(const Shader* shader) const
	{
		if (positionsLine.size() == 0)
			return;

		ScopedBindArray ba(linesVAO);
		setColor(shader);
		glDrawArrays(GL_LINES, 0, positionsLine.size());
	}

	virtual inline void drawAdditionalPoints(const Shader* shader) const
	{
		// Unnecessary in base implementation
	}

	virtual void drawPolygon(const Shader* shader) const
	{
		shader->setVector("color", polygonColor);
		shader->setMatrix("model", glm::mat4{1.0f});
		ScopedBindArray ba(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, positions.size());
	}

	virtual ~Curve()
	{
		for (auto&& point : points)
		{
			point->removeFromCurve(this);
		}
	}

	void addPoints(Point* toAdd)
	{
		points.push_back(toAdd);
		toAdd->addToCurve(this);
		scheduleToUpdate();
	}

	void addPoints(const std::vector<Point*>& toAdd)
	{
		for (auto&& point : toAdd)
		{
			points.push_back(point);
			point->addToCurve(this);
		}
		scheduleToUpdate();
	}

	void removePoints()
	{
		for (auto&& point : points)
		{
			if (point->isSelected)
				point->removeFromCurve(this);
		}
		std::erase_if(points, [](auto&& point) { return point->isSelected; });
		scheduleToUpdate();
	}

	void scheduleToUpdate(const Point* invoker = nullptr)
	{
		scheduledToUpdate = true;
		if (invoker)
			updateInvoker = invoker;
	}

	virtual void update()
	{
		// Nothing happens without any connectivity constraints
		scheduledToUpdate = true;
	}

	virtual inline const std::vector<std::unique_ptr<Point>>& getVirtualPoints() const
	{
		static const std::vector<std::unique_ptr<Point>> empty;
		return empty;
	}

	virtual inline void updateRenderer()
	{

	}

	glm::vec3 getCenter()
	{
		return std::accumulate(positions.begin(), positions.end(), glm::vec3{ 0.0f, 0.0f, 0.0f }) * (1.0f / positions.size());
	}

protected:
	static constexpr int degree = 3;

	inline static unsigned int instanceCount = 0;
	inline static const glm::vec4 polygonColor = colors::orange();

	std::vector<Point*> points;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> positionsQuadratic;
	std::vector<glm::vec3> positionsLine;

	bool scheduledToUpdate = true;
	const Point* updateInvoker = nullptr;

	Curve(const std::string& curveName, const std::vector<Point*>& points) : Selectable(std::format("{} {}", curveName, instanceCount++)), points(points)
	{
		attachPointsToCurve();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &quadraticVAO);
		glGenBuffers(1, &quadraticVBO);
		glGenVertexArrays(1, &linesVAO);
		glGenBuffers(1, &linesVBO);

		update();
	}

	inline virtual std::string getCurveName() const = 0;

	virtual void fillPositions() = 0;

	void uploadPositions()
	{
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
		{
			ScopedBindArray ba(quadraticVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadraticVBO);
			if (positionsQuadratic.size() > 0)
			{
				glBufferData(GL_ARRAY_BUFFER, positionsQuadratic.size() * sizeof(glm::vec3), &positionsQuadratic[0], GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			}
			else
			{
				glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
			}
		}
		{
			ScopedBindArray ba(linesVAO);
			glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
			if (positionsLine.size() > 0)
			{
				glBufferData(GL_ARRAY_BUFFER, positionsLine.size() * sizeof(glm::vec3), &positionsLine[0], GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			}
			else
			{
				glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
			}
		}
	}

private:
	unsigned int quadraticVAO, linesVAO = 0;
	unsigned int quadraticVBO, linesVBO = 0;

	void attachPointsToCurve()
	{
		for (auto&& point : points)
		{
			point->addToCurve(this);
		}
	}
};