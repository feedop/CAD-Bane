export module texturedobject;

import std;
import glm;

import <glad/glad.h>;
import <cstddef>;

import glutils;
import solidobject;
import shader;

/// <summary>
/// A class representing a 3D textured object that can be rendered with a texture applied.
/// It inherits from <see cref="SolidObject"/> and adds functionality for handling texture coordinates
/// and binding texture data to the object for rendering with textures.
/// </summary>
export class TexturedObject : public SolidObject
{
protected:
	/// <summary>
	/// Structure representing a vertex with a position and texture coordinates.
	/// </summary>
	struct Vertex
	{
		glm::vec3 translation;
		glm::vec2 texCoords;
	};

public:
	/// <summary>
	/// Constructor that initializes the textured object with given vertices and indices.
	/// It sets up the VAO, VBO, and EBO for rendering the object with the texture.
	/// </summary>
	/// <param name="vertices">The list of vertices, including positions and texture coordinates.</param>
	/// <param name="indices">The list of indices for drawing the object using the vertex array.</param>
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

	/// <summary>
	/// Draws the textured object using the provided shader. 
	/// </summary>
	/// <param name="shader">The shader to use for rendering the object.</param>
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