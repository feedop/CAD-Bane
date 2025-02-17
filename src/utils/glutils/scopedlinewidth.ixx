export module glutils:scopedlinewidth;

import <glad/glad.h>;

/// <summary>
/// Sets and unsets OpenGL line width.
/// </summary>
export struct ScopedLineWidth
{
	ScopedLineWidth(float width)
	{
		glLineWidth(width);
	}

	~ScopedLineWidth()
	{
		glLineWidth(1.0f);
	}
};