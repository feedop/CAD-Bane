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

export class Point : public SolidObject, public Selectable, public Clickable
{
	
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
	};

	Point(const glm::vec3& translation = glm::vec3{ 0.0f, 0.0f, 0.0f }, bool isVirtual = false) : SolidObject(translation), Selectable(std::format("Point {}", instanceCount))
	{
		if (!isVirtual)
			instanceCount++;
	}

	Point(const glm::vec3& translation, const std::string& name) : SolidObject(translation), Selectable(name)
	{
		instanceCount++;
	}

	Point(const MG1::Point& other) : Point(toGLM(other.position))
	{
		if (!other.name.empty())
			setName(other.name);

		id = other.GetId();
	}

	operator MG1::Point() const
	{
		MG1::Point p;
		p.position = toMG1(position);
		p.name = getName();
		return p;
	}

	Vertex toVertex() const
	{
		return {
			position,
			isSelected ? selectedColor : color
		};
	}

	virtual bool isCoordInObject(const glm::vec3& coord) const override
	{
		float diff = glm::length(position - coord);
		return (diff < 0.1f);
	}

	virtual void translate(const glm::vec3& positionChange) override
	{
		if (movedThisFrame)
			return;
		SolidObject::translate(positionChange);
		update();
		movedThisFrame = true;
	}

	virtual void rotateLocal(float angleRadians, const glm::vec3& axis) override
	{
		// Not needed for points
	}

	virtual void rotateGlobal(float angleRadians, const glm::vec3& axis, const glm::vec3& centerPoint) override
	{
		auto rotationQuat = math::Quat::angleAxis(angleRadians, axis);
		rotation = (rotationQuat * rotation).normalized();
		position = centerPoint + glm::vec3(rotationQuat.toMatrix() * glm::vec4(position - centerPoint, 0.0f));
		update();
	}

	virtual void scaleLocal(const glm::vec3& direction) override
	{
		// Not needed for points
	}

	virtual void scaleGlobal(const glm::vec3& direction, const glm::vec3& centerPoint) override
	{
		SolidObject::scaleGlobal(direction, centerPoint);
		update();
	}

	inline void attach(Shape* shape)
	{
		attachedTo.insert(shape);
	}

	inline void detach(Shape* shape)
	{
		attachedTo.erase(shape);
	}

	inline bool isAttached() const
	{
		return attachedTo.size() > 0;
	}

	inline const auto& getAttachmentList() const
	{
		return attachedTo;
	}

	inline glm::vec3 getLastTranslation() const
	{
		return position - lastPosition;
	}

	inline unsigned int getId() const
	{
		return id;
	}

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