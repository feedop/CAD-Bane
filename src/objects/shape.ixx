export module shape;

import std;

import drawable;
import point;
import selectable;
import updatable;

/// <summary>
/// Represents a shape that can be drawn, selected, and updated. The shape is composed of points, which are 
/// managed and updated as part of the shape. This class is derived from <see cref="Drawable"/>, <see cref="Selectable"/>, 
/// and <see cref="Updatable"/> to provide drawing, selection, and updating functionalities for the shape.
/// </summary>
export class Shape : public Drawable, public Selectable, public Updatable
{
public:
	/// <summary>
	/// Constructs a Shape object with a specified name.
	/// </summary>
	/// <param name="name">The name of the shape.</param>
	Shape(const std::string& name) : Selectable(name)
	{}

	/// <summary>
	/// Constructs a Shape object with a specified name and a list of points.
	/// </summary>
	/// <param name="name">The name of the shape.</param>
	/// <param name="points">A vector of pointers to the points that make up the shape.</param>
	Shape(const std::string& name, const std::vector<Point*>& points) : Selectable(name), points(points)
	{}

	/// <summary>
	/// Destructor that detaches the shape from its points when destroyed.
	/// </summary>
	virtual ~Shape()
	{
		for (auto&& point : points)
		{
			point->detach(this);
		}
	}

	/// <summary>
	/// Collapses two points of the shape into a single new point.
	/// </summary>
	/// <param name="oldPoint1">The first point to collapse.</param>
	/// <param name="oldPoint2">The second point to collapse.</param>
	/// <param name="newPoint">The new point that replaces the collapsed points.</param>
	virtual void collapsePoints(const Point* oldPoint1, const Point* oldPoint2, Point* newPoint)
	{
		auto it = std::find(points.begin(), points.end(), oldPoint1);
		if (it != points.end())
			*it = newPoint;
		points.erase(std::remove(points.begin(), points.end(), oldPoint2), points.end());

		newPoint->attach(this);
	}

protected:
	std::vector<Point*> points;

	/// <summary>
	/// Attaches the points to this shape.
	/// </summary>
	void attachPoints()
	{
		for (auto&& point : points)
		{
			point->attach(this);
		}
	}
};