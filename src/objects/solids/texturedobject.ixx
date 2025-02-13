export module texturedobject;

import std;
import glm;

import <glad/glad.h>;
import <cstddef>;

import glutils;
import solidobject;
import shader;

export class TexturedObject : public SolidObject
{
protected:
	struct Vertex
	{
		glm::vec3 translation;
		glm::vec2 texCoords;
	};

public:
	TexturedObject(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices) :
		vertices(vertices), indices(indices)
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
		// texture coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	}

	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		SolidObject::draw(shader);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	}

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};