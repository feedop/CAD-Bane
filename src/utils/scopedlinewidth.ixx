export module glutils:scopedlinewidth;

import <glad/glad.h>;

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