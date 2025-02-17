export module pointrenderer;

import std;
import glm;

import <glad/glad.h>;
import <cstddef>;

import drawable;
import glutils;
import middlepoint;
import point;
import shader;

/// <summary>
/// A renderer for drawing points in OpenGL.
/// </summary>
export class PointRenderer : public Drawable
{
public:
	/// <summary>
	/// Constructs a new PointRenderer, initializing OpenGL buffers.
	/// </summary>
	PointRenderer()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}

	/// <summary>
	/// Updates the point positions in the renderer without including the selected middle point.
	/// </summary>
	/// <param name="points">A collection of points to be rendered.</param>
	void updateSoft(const std::vector<std::unique_ptr<Point>>& points)
	{
		positions.clear();
		for (auto&& point : points)
		{
			positions.push_back(point.get()->toVertex());
		}

		uploadPositions();
	}

	/// <summary>
	/// Updates the point positions in the renderer, including the selected middle point.
	/// </summary>
	/// <param name="points">A collection of points to be rendered.</param>
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

	/// <summary>
	/// Draws the points using the specified shader.
	/// </summary>
	/// <param name="shader">The shader used for rendering.</param>
	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		glDrawArrays(GL_POINTS, 0, positions.size());
	}

private:
	std::vector<Point::Vertex> positions;

	/// <summary>
	/// Uploads point positions to the GPU.
	/// </summary>
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