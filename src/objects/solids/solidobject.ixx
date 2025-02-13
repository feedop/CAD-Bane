export module solidobject;

import std;
import glm;

import <glad/glad.h>;

import drawable;
import math;
import shader;


export class SolidObject : public Drawable
{
public:

	inline const glm::vec3& getPosition() const
	{
		return position;
	}

	inline void setPosition(const glm::vec3& newPosition)
	{
		position = newPosition;
	}

	virtual void draw(const Shader* shader) const override
	{
		shader->setMatrix("model", model);
	}

	virtual void translate(const glm::vec3& positionChange)
	{
		position += positionChange;
		update();
	}

	virtual void rotateLocal(float angleRadians, const glm::vec3& axis)
	{
		rotation = (math::Quat::angleAxis(angleRadians, axis) * rotation).normalized();
		update();
	}

	virtual void rotateGlobal(float angleRadians, const glm::vec3& axis, const glm::vec3& centerPoint)
	{
		auto rotationQuat = math::Quat::angleAxis(angleRadians, axis);
		rotation = (rotationQuat * rotation).normalized();
		position = centerPoint + glm::vec3(rotationQuat.toMatrix() * glm::vec4(position - centerPoint, 0.0f));
		update();
	}

	virtual void scaleLocal(const glm::vec3& direction)
	{
		glm::vec3 rotatedDir = glm::inverse(rotation.toMatrix()) * glm::vec4(direction, 0.0f);
		scale += rotatedDir;
		update();
	}

	virtual void scaleGlobal(const glm::vec3& direction, const glm::vec3& centerPoint)
	{
		glm::vec3 rotatedDir = glm::length(direction) * glm::normalize(glm::inverse(rotation.toMatrix()) * glm::vec4(direction, 0.0f));
		auto prevScaleMatrix = rotation.toMatrix() * math::scale(scale);
		auto newScaleMatrix = rotation.toMatrix() * math::scale(scale + rotatedDir);
		scale += rotatedDir;

		position = centerPoint + glm::vec3(newScaleMatrix * glm::inverse(prevScaleMatrix) * glm::vec4(position - centerPoint, 0.0f));
		update();
	}

protected:

	glm::mat4 model{ 1.0f };

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	math::Quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

	SolidObject() = default;
	SolidObject(const glm::vec3& position) : position(position)
	{
		update();
	}

	SolidObject(const glm::vec3& position, const math::Quat& rotation, const glm::vec3& scale) : position(position), rotation(rotation), scale(scale)
	{
		update();
	}

	virtual void update()
	{
		model = math::translate(position) * rotation.toMatrix() * math::scale(scale);
	}
};