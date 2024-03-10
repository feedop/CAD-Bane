export module checkergrid;

import canvas;
import math;

export class CheckerGrid : public Canvas
{
public:
	CheckerGrid()
	{
		model = math::scale({ 10.0f, 10.0f, 10.0f }) * math::rotate(-math::pi / 2, { 1.0f, 0.0, 0.0 });
	}	
};