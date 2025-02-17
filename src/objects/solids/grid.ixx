export module grid;

import canvas;
import math;

/// <summary>
/// Represents a 3D grid displayed on a canvas. The grid is scaled and rotated to visualize a coordinate system 
/// in 3D space, often used in graphical applications or editors for alignment or reference purposes.
/// </summary>
export class Grid : public Canvas
{
public:
	/// <summary>
	/// Constructor that initializes the grid with a transformation that ensures proper displaying on the screen.
	/// </summary>
	Grid()
	{
		model = math::scale({ 10.0f, 10.0f, 10.0f }) * math::rotate(-math::pi / 2, { 1.0f, 0.0, 0.0 });
	}
};