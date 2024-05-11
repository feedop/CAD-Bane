export module point;

import <format>;
import <unordered_set>;

import <glm/vec3.hpp>;

import clickable;
import colors;
import selectable;
import solidobject;
import math;

export class Updatable;

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
		SolidObject::translate(positionChange);
		update();
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

	inline void attach(Updatable* updatable)
	{
		attachedTo.insert(updatable);
	}

	inline void detach(Updatable* updatable)
	{
		attachedTo.erase(updatable);
	}

	inline bool isAttached() const
	{
		return attachedTo.size() > 0;
	}

	inline const glm::vec3& getLastTranslation() const
	{
		return position - lastPosition;
	}

protected:
	std::unordered_set<Updatable*> attachedTo;
	glm::vec3 lastPosition{ 0.0f, 0.0f, 0.0f };

	virtual void update() override;

private:
	friend class GuiController;
	inline static unsigned int instanceCount = 0;
};