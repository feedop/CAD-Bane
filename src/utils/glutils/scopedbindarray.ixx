export module glutils:scopedbindarray;

import <glad/glad.h>;

export struct ScopedBindArray
{
	ScopedBindArray(unsigned int VAO)
	{
		glBindVertexArray(VAO);
	}

	~ScopedBindArray()
	{
		glBindVertexArray(0);
	}
};