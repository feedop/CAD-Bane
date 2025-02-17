export module cursor;

import std;
import glm;

import <glad/glad.h>;
import <cstddef>;

import glutils;
import solidobject;
import shader;
import math;

/// <summary>
/// Represents a cursor object in 3D space, visualized as three orthogonal colored lines 
/// emanating from the origin (0, 0, 0). This object is primarily used for visualizing the position
/// of a cursor in 3D environments, such as in 3D editors or interactive applications.
/// </summary>
export class Cursor : public SolidObject
{
	/// <summary>
	/// Represents a vertex containing a position and color.
	/// </summary>
	struct Vertex
	{
		glm::vec3 translation;
		glm::vec3 color;
	};

public:
	/// <summary>
	/// Constructor that sets up the necessary OpenGL buffers (VAO, VBO, EBO) for the cursor's geometry and color.
	/// The cursor consists of 3 orthogonal lines representing the X, Y, and Z axes, each with a different color.
	/// </summary>
	Cursor()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
					 &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// colors
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		scale = { 0.1f, 0.1f, 0.1f };
		update();
	}

	/// <summary>
	/// Draws the cursor object using the specified shader. The cursor is rendered as lines, with each line representing
	/// one of the X, Y, or Z axes in different colors.
	/// </summary>
	/// <param name="shader">The shader program used for drawing the cursor.</param>
	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		SolidObject::draw(shader);
		glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	}

private:
	inline static constexpr std::array<Vertex, 6> vertices{ {
		{
			{0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f}
		},
		{
			{1.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f}
		},
		{
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}
		},
		{
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}
		},
		{
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
		},
		{
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f}
		}
	} };
	inline static constexpr std::array<unsigned int, 6> indices{ {
		0, 1,
		2, 3,
		4, 5
	} };
};