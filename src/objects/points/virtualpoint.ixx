export module virtualpoint;

import colors;
import point;
import shape;

/// <summary>
/// Represents a virtual point in 3D space that is similar to a regular point but does not affect the count of instances and is owned by another object.
/// It can be translated, and it updates the attached shapes when it changes position.
/// </summary>
export class VirtualPoint : public Point
{
public:
	/// <summary>
	/// Constructor that initializes the virtual point with a translation vector.
	/// The color of the virtual point is set to orange by default.
	/// </summary>
	/// <param name="translation">The translation vector that defines the position of the virtual point.</param>
	VirtualPoint(const glm::vec3& translation = glm::vec3{ 0.0f, 0.0f, 0.0f }) : Point(translation, true)
	{
		color = colors::orange;
	}

	/// <summary>
	/// Translates the virtual point by a given position change, updating its position and attached shapes.
	/// </summary>
	/// <param name="positionChange">The translation vector applied to the point.</param>
	virtual void translate(const glm::vec3& positionChange) override
	{
		lastPosition = position;
		Point::translate(positionChange);
		update();
	}

	/// <summary>
	/// Updates the attached shapes by scheduling them for an update when the virtual point's position changes.
	/// </summary>
	virtual void update() override
	{
		for (auto&& shape : attachedTo)
		{
			shape->scheduleToUpdate(this);
		}
	}
};