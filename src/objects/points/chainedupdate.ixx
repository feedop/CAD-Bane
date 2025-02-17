export module chainedupdate;

import point;
import shape;

/// <summary>
/// Update a point in case of a chained update
/// </summary>
export void Point::update()
{
	for (auto&& shape : attachedTo)
	{
		shape->scheduleToUpdate();
	}
}