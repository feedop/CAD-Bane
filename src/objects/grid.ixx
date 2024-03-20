export module grid;

import canvas;
import math;

export class Grid : public Canvas
{
public:
	Grid()
	{
		model = math::scale({ 10.0f, 10.0f, 10.0f }) * math::rotate(-math::pi / 2, { 1.0f, 0.0, 0.0 });
	}
};