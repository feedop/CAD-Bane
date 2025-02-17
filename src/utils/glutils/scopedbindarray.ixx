export module glutils:scopedbindarray;

import <glad/glad.h>;

/// <summary>
/// Sets and unsets an OpenGL Vertex Array Object.
/// </summary>
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