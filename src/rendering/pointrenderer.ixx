export module pointrenderer;

import <glad/glad.h>;
import <memory>;
import <vector>;

import <glm/vec3.hpp>;
import <glm/mat4x4.hpp>;

import drawable;
import glutils;
import middlepoint;
import point;
import shader;

export class PointRenderer : public Drawable
{
public:
	PointRenderer()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}

	void updateSoft(const std::vector<std::unique_ptr<Point>>& points)
	{
		positions.clear();
		for (auto&& point : points)
		{
			positions.push_back(point.get()->toVertex());
		}

		uploadPositions();
	}

	void update(const std::vector<std::unique_ptr<Point>>& points)
	{
		positions.clear();
		const auto& mp = MiddlePoint::getInstance();
		if (mp.isSelected)
			positions.push_back(mp.toVertex());
		for (auto&& point : points)
		{
			positions.push_back(point->toVertex());
		}

		uploadPositions();
	}

	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		glDrawArrays(GL_POINTS, 0, positions.size());
	}

private:

	std::vector<Point::Vertex> positions;

	void uploadPositions()
	{
		if (positions.size() == 0)
			return;

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Point::Vertex), &positions[0], GL_DYNAMIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point::Vertex), (void*)0);
		// colors
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point::Vertex), (void*)offsetof(Point::Vertex, color));
	}
};