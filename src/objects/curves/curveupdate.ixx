export module curveupdate;

import point;
import curve;

export void Point::update()
{
	for (auto&& curve : inCurves)
	{
		curve->scheduleToUpdate();
	}
}