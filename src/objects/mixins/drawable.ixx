export module drawable;

import <glad/glad.h>;

import glutils;
import shader;

export class Drawable
{
public:
	virtual ~Drawable()
	{
		if (VAO == 0)
			return;

		ScopedBindArray ba(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDeleteBuffers(1, &VBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDeleteBuffers(1, &EBO);

		glDeleteVertexArrays(1, &VAO);
	}

	virtual void draw(const Shader* shader) const = 0;

	Drawable() = default;
	Drawable(const Drawable& other) = delete;
	Drawable& operator=(const Drawable& other) = delete;

protected:
	unsigned int VAO = 0, VBO = 0, EBO = 0;
};