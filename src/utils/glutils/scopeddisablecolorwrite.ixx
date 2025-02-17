export module glutils:scopeddisablecolorwrite;

import <glad/glad.h>;

/// <summary>
/// Disables and re-enables OpenGl color write
/// </summary>
export struct ScopedDisableColorWrite
{
	ScopedDisableColorWrite()
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}

	~ScopedDisableColorWrite()
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
};