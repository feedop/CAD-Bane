export module chainedupdate;

import point;
import updatable;

export void Point::update()
{
	for (auto&& updatable : attachedTo)
	{
		updatable->scheduleToUpdate();
	}
}