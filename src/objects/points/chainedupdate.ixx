export module chainedupdate;

import point;
import shape;

export void Point::update()
{
	for (auto&& shape : attachedTo)
	{
		shape->scheduleToUpdate();
	}
}