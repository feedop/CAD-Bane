export module virtualpoint;

import colors;
import point;
import shape;

export class VirtualPoint : public Point
{
public:
	VirtualPoint(const glm::vec3& translation = glm::vec3{ 0.0f, 0.0f, 0.0f }) : Point(translation, true)
	{
		color = colors::orange;
	}

	virtual void translate(const glm::vec3& positionChange) override
	{
		lastPosition = position;
		Point::translate(positionChange);
		update();
	}

	virtual void update() override
	{
		for (auto&& shape : attachedTo)
		{
			shape->scheduleToUpdate(this);
		}
	}
};