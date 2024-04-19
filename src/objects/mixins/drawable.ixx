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

protected:
	unsigned int VAO = 0, VBO = 0, EBO = 0;
};