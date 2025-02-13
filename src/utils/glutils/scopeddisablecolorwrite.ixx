export module glutils:scopeddisablecolorwrite;

import <glad/glad.h>;

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