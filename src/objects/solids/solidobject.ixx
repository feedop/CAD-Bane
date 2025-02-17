export module solidobject;

import std;
import glm;

import <glad/glad.h>;

import drawable;
import math;
import shader;

/// <summary>
/// A class representing a 3D solid object that can be translated, rotated, and scaled. 
/// It also contains methods for manipulating the object's position, rotation, and scale, 
/// and updating its transformation matrix for rendering purposes.
/// </summary>
export class SolidObject : public Drawable
{
public:

	/// <summary>
	/// Gets the position of the object in 3D space.
	/// </summary>
	/// <returns>The position of the object.</returns>
	inline const glm::vec3& getPosition() const
	{
		return position;
	}

	/// <summary>
	/// Sets the position of the object in 3D space.
	/// </summary>
	/// <param name="newPosition">The new position to set for the object.</param>
	inline void setPosition(const glm::vec3& newPosition)
	{
		position = newPosition;
	}

	/// <summary>
	/// Draws the object using the provided shader. 
	/// </summary>
	/// <param name="shader">The shader to use for rendering the object.</param>
	virtual void draw(const Shader* shader) const override
	{
		shader->setMatrix("model", model);
	}

	/// <summary>
	/// Translates the object by the specified amount.
	/// This modifies the object's position and updates its transformation matrix.
	/// </summary>
	/// <param name="positionChange">The amount by which to translate the object.</param>
	virtual void translate(const glm::vec3& positionChange)
	{
		position += positionChange;
		update();
	}

	/// <summary>
	/// Rotates the object locally by a given angle around a specified axis.
	/// This modifies the object's rotation and updates its transformation matrix.
	/// </summary>
	/// <param name="angleRadians">The angle in radians by which to rotate the object.</param>
	/// <param name="axis">The axis around which to rotate the object.</param>
	virtual void rotateLocal(float angleRadians, const glm::vec3& axis)
	{
		rotation = (math::Quat::angleAxis(angleRadians, axis) * rotation).normalized();
		update();
	}

	/// <summary>
	/// Rotates the object globally by a given angle around a specified axis and a center point.
	/// This modifies the object's position and rotation, and updates its transformation matrix.
	/// </summary>
	/// <param name="angleRadians">The angle in radians by which to rotate the object.</param>
	/// <param name="axis">The axis around which to rotate the object.</param>
	/// <param name="centerPoint">The center point around which to rotate the object.</param>
	virtual void rotateGlobal(float angleRadians, const glm::vec3& axis, const glm::vec3& centerPoint)
	{
		auto rotationQuat = math::Quat::angleAxis(angleRadians, axis);
		rotation = (rotationQuat * rotation).normalized();
		position = centerPoint + glm::vec3(rotationQuat.toMatrix() * glm::vec4(position - centerPoint, 0.0f));
		update();
	}

	/// <summary>
	/// Scales the object locally by a given amount along the specified direction.
	/// This modifies the object's scale and updates its transformation matrix.
	/// </summary>
	/// <param name="direction">The direction and amount by which to scale the object.</param>
	virtual void scaleLocal(const glm::vec3& direction)
	{
		glm::vec3 rotatedDir = glm::inverse(rotation.toMatrix()) * glm::vec4(direction, 0.0f);
		scale += rotatedDir;
		update();
	}

	/// <summary>
	/// Scales the object globally by a given amount along the specified direction and around a center point.
	/// This modifies the object's position and scale, and updates its transformation matrix.
	/// </summary>
	/// <param name="direction">The direction and amount by which to scale the object.</param>
	/// <param name="centerPoint">The center point around which to scale the object.</param>
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

	/// <summary>
	/// Default constructor that initializes the object with a position of (0, 0, 0).
	/// </summary>
	SolidObject() = default;

	/// <summary>
	/// Constructor that initializes the object with a given position.
	/// </summary>
	/// <param name="position">The position to set for the object.</param>
	SolidObject(const glm::vec3& position) : position(position)
	{
		update();
	}

	/// <summary>
	/// Constructor that initializes the object with a given position, rotation, and scale.
	/// </summary>
	/// <param name="position">The position to set for the object.</param>
	/// <param name="rotation">The rotation to set for the object as a quaternion.</param>
	/// <param name="scale">The scale to set for the object.</param>
	SolidObject(const glm::vec3& position, const math::Quat& rotation, const glm::vec3& scale) : position(position), rotation(rotation), scale(scale)
	{
		update();
	}

	/// <summary>
	/// Updates the model transformation matrix based on the current position, rotation, and scale.
	/// This method is called whenever the object's transformation changes.
	/// </summary>
	virtual void update()
	{
		model = math::translate(position) * rotation.toMatrix() * math::scale(scale);
	}
};