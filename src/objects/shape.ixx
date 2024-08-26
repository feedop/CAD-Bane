export module shape;

import <algorithm>;
import <vector>;

import drawable;
import point;
import selectable;
import updatable;

export class Shape : public Drawable, public Selectable, public Updatable
{
public:
	Shape(const std::string& name) : Selectable(name)
	{}

	Shape(const std::string& name, const std::vector<Point*>& points) : Selectable(name), points(points)
	{}

	virtual ~Shape()
	{
		for (auto&& point : points)
		{
			point->detach(this);
		}
	}

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

	void attachPoints()
	{
		for (auto&& point : points)
		{
			point->attach(this);
		}
	}
};