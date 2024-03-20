export module cursor;

import <glad/glad.h>;
import <array>;

import <glm/vec3.hpp>;

import object;
import shader;
import math;

export class Cursor : public Object
{
	struct Vertex
	{
		glm::vec3 translation;
		glm::vec3 color;
	};

public:
	Cursor()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
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

		glBindVertexArray(0);

		localScale = { 0.1f, 0.1f, 0.1f };
		update();
	}

	virtual void draw(const Shader* shader) const override
	{
		Object::draw(shader);
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
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