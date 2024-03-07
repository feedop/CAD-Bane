export module canvas;

import <glad/glad.h>;
import <array>;

import <glm/vec2.hpp>;
import <glm/vec3.hpp>;

import object;
import shader;


export class Canvas : public Object
{
public:
	Canvas()
	{
		init();
	}

	Canvas(const glm::mat4& model) : Object(model)
	{
		init();
	}

	virtual ~Canvas() override
	{
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDeleteBuffers(1, &VBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDeleteBuffers(1, &EBO);

		glDeleteVertexArrays(1, &VAO);
		glBindVertexArray(0);
	}

	virtual void draw(const Shader* shader) const override
	{
		Object::draw(shader);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	static constexpr std::array<Vertex, 4> vertices = { {
		{
			{-1.0f, 1.0f, 0.0f},
			{ 0.0f, 0.0f }
		},
		{
			{1.0f, 1.0f, 0.0f},
			{0.0f, 1.0f}
		},
		{
			{-1.0f, -1.0f, 0.0f},
			{1.0f, 0.0f}
		},
		{
			{1.0f, -1.0f, 0.0f},
			{1.0f, 1.0f}
		}
	} };

	static constexpr std::array<unsigned int, 6> indices
	{
		0, 1, 2,
		1, 3, 2
	};

	void init()
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
		// texture coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glBindVertexArray(0);
	}
};