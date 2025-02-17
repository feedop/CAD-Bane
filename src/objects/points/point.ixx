export module point;

import std;
import glm;

import <Serializer/Serializer.h>;

import clickable;
import colors;
import selectable;
import solidobject;
import math;
import mg1utils;

export class Shape;

/// <summary>
/// Represents a point object in 3D space. It can be translated, attached to shapes, and be selected or clicked.
/// The class provides functionality to store and manipulate the position of the point, and track whether it's moved.
/// </summary>
export class Point : public SolidObject, public Selectable, public Clickable
{
	
public:
	// <summary>
	/// A structure representing a vertex for the point, containing position and color.
	/// </summary>
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
	};

	// <summary>
	/// Constructor that initializes the point with a translation and an optional flag to make it virtual.
	/// </summary>
	/// <param name="translation">The translation of the point in 3D space.</param>
	/// <param name="isVirtual">A flag that specifies if the point is virtual (does not increment the instance count).</param>
	Point(const glm::vec3& translation = glm::vec3{ 0.0f, 0.0f, 0.0f }, bool isVirtual = false) : SolidObject(translation), Selectable(std::format("Point {}", instanceCount))
	{
		if (!isVirtual)
			instanceCount++;
	}

	/// <summary>
	/// Constructor that initializes the point with a translation and a custom name.
	/// </summary>
	/// <param name="translation">The translation of the point in 3D space.</param>
	/// <param name="name">The name of the point.</param>
	Point(const glm::vec3& translation, const std::string& name) : SolidObject(translation), Selectable(name)
	{
		instanceCount++;
	}

	/// <summary>
	/// Constructor that deserializes the point from an MG1::Point object.
	/// </summary>
	/// <param name="other">The MG1::Point object used to initialize the point.</param>
	Point(const MG1::Point& other) : Point(toGLM(other.position))
	{
		if (!other.name.empty())
			setName(other.name);

		id = other.GetId();
	}

	/// <summary>
	/// Serializes the Point object to an MG1::Point object.
	/// </summary>
	/// <returns>The converted MG1::Point object.</returns>
	operator MG1::Point() const
	{
		MG1::Point p;
		p.position = toMG1(position);
		p.name = getName();
		return p;
	}

	/// <summary>
	/// Converts the point to a Vertex object with position and color.
	/// </summary>
	/// <returns>The corresponding Vertex object representing the point.</returns
	Vertex toVertex() const
	{
		return {
			position,
			isSelected ? selectedColor : color
		};
	}

	/// <summary>
	/// Checks if the given coordinates are within a certain threshold of the point.
	/// </summary>
	/// <param name="coord">The coordinates to check.</param>
	/// <returns>True if the coordinates are close to the point, otherwise false.</returns>
	virtual bool isCoordInObject(const glm::vec3& coord) const override
	{
		float diff = glm::length(position - coord);
		return (diff < 0.1f);
	}

	/// <summary>
	/// Translates the point by a given position change.
	/// </summary>
	/// <param name="positionChange">The translation vector applied to the point.</param>
	virtual void translate(const glm::vec3& positionChange) override
	{
		if (movedThisFrame)
			return;
		SolidObject::translate(positionChange);
		update();
		movedThisFrame = true;
	}

	/// <summary>
	/// Not needed for points.
	/// </summary>
	virtual void rotateLocal(float angleRadians, const glm::vec3& axis) override
	{
		
	}

	/// <summary>
	/// Rotates the point globally around a specified center point by a given angle.
	/// </summary>
	/// <param name="angleRadians">The angle to rotate by, in radians.</param>
	/// <param name="axis">The axis to rotate around.</param>
	/// <param name="centerPoint">The center point around which to rotate.</param>
	virtual void rotateGlobal(float angleRadians, const glm::vec3& axis, const glm::vec3& centerPoint) override
	{
		auto rotationQuat = math::Quat::angleAxis(angleRadians, axis);
		rotation = (rotationQuat * rotation).normalized();
		position = centerPoint + glm::vec3(rotationQuat.toMatrix() * glm::vec4(position - centerPoint, 0.0f));
		update();
	}

	/// <summary>
	/// Not needed for points.
	/// </summary>
	/// <param name="direction"></param>
	virtual void scaleLocal(const glm::vec3& direction) override
	{

	}

	/// <summary>
	/// Scales the point globally by a given scaling direction and center point.
	/// </summary>
	/// <param name="direction">The scaling direction vector.</param>
	/// <param name="centerPoint">The center point around which to scale.</param>
	virtual void scaleGlobal(const glm::vec3& direction, const glm::vec3& centerPoint) override
	{
		SolidObject::scaleGlobal(direction, centerPoint);
		update();
	}

	/// <summary>
	/// Attaches the point to a given shape.
	/// </summary>
	/// <param name="shape">The shape to attach the point to.</param>
	inline void attach(Shape* shape)
	{
		attachedTo.insert(shape);
	}

	/// <summary>
	/// Detaches the point from a given shape.
	/// </summary>
	/// <param name="shape">The shape to detach the point from.</param>
	inline void detach(Shape* shape)
	{
		attachedTo.erase(shape);
	}

	/// <summary>
	/// Checks if the point is attached to any shape.
	/// </summary>
	/// <returns>True if the point is attached to any shape, otherwise false.</returns>
	inline bool isAttached() const
	{
		return attachedTo.size() > 0;
	}

	/// <summary>
	/// Retrieves the list of shapes that the point is attached to.
	/// </summary>
	/// <returns>A constant reference to the set of attached shapes.</returns>
	inline const auto& getAttachmentList() const
	{
		return attachedTo;
	}

	/// <summary>
	/// Retrieves the last translation vector of the point.
	/// </summary>
	/// <returns>The translation vector from the previous frame.</returns>
	inline glm::vec3 getLastTranslation() const
	{
		return position - lastPosition;
	}

	/// <summary>
	/// Retrieves the unique ID of the point.
	/// </summary>
	/// <returns>The unique ID of the point.</returns>
	inline unsigned int getId() const
	{
		return id;
	}

	/// <summary>
	/// Resets the moved state of the point, indicating it has not been moved this frame.
	/// </summary>
	inline void resetMoved()
	{
		movedThisFrame = false;
	}

protected:
	std::unordered_set<Shape*> attachedTo;
	glm::vec3 lastPosition{ 0.0f, 0.0f, 0.0f };
	bool movedThisFrame = false;

	virtual void update() override;

private:
	friend class GuiController;
	inline static unsigned int instanceCount = 0;

	unsigned int id = 0;
};