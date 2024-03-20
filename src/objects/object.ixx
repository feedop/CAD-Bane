export module object;

import <glad/glad.h>;
import <vector>;

import <glm/mat4x4.hpp>;

import shader;
import math;

export class Object
{
public:
	virtual ~Object()
	{
		if (VAO == 0)
			return;

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDeleteBuffers(1, &VBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDeleteBuffers(1, &EBO);

		glDeleteVertexArrays(1, &VAO);
		glBindVertexArray(0);
	}

	inline const glm::vec3& getPosition() const
	{
		return position;
	}

	virtual void draw(const Shader* shader) const
	{
		shader->setMatrix("model", model);
	}

	void translate(const glm::vec3& positionChange)
	{
		glm::vec3 change = /*glm::inverse(globalRotation) **/ glm::vec4(positionChange, 1.0f);
		translation += change;
		update();
	}

	void rotateLocal(float angleRadians, const glm::vec3& axis)
	{
		localRotation = math::rotate(angleRadians, axis) * localRotation;
		update();
	}

	void rotateGlobal(float angleRadians, const glm::vec3& axis, const glm::vec3& centerPoint)
	{
		globalTransform = math::translate(centerPoint - translation) *
			math::rotate(angleRadians, axis) *
			math::translate(-centerPoint + translation) *
			globalTransform;
		update();
	}

	void scaleLocal(const glm::vec3& coefficient)
	{
		localScale += coefficient;
		update();
	}

	void scaleGlobal(const glm::vec3& axis, const glm::vec3& centerPoint)
	{
		globalTransform = math::translate(centerPoint - translation) *
			math::scale(axis) *
			math::translate(-centerPoint + translation) *
			globalTransform;
		update();
	}

protected:
	unsigned int VAO = 0, VBO = 0, EBO = 0;

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::mat4 model{ 1.0f };

	glm::vec3 translation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 localScale{ 1.0f, 1.0f, 1.0f };

	glm::mat4 localRotation{ 1.0f };
	glm::mat4 globalTransform{ 1.0f };

	Object() = default;
	Object(const glm::vec3& translation) : translation(translation)
	{
		update();
	}

	virtual void update()
	{
		model = math::translate(translation) * globalTransform * localRotation * math::scale(localScale);
		position = model * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
	}
};