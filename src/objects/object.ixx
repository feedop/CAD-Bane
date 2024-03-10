export module object;

import <glad/glad.h>;
import <string>;
import <vector>;

import <glm/mat4x4.hpp>;

import shader;
import math;

export class Object
{
public:
	bool isSelected = false;

	virtual ~Object()
	{
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDeleteBuffers(1, &VBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDeleteBuffers(1, &EBO);

		glDeleteVertexArrays(1, &VAO);
		glBindVertexArray(0);
	}

	virtual void draw(const Shader* shader) const
	{
		shader->setMatrix("model", model);
	}

	std::string getName()
	{
		return name;
	}

	void translate(const glm::vec3 translation)
	{
		position += translation;
		update();
	}

protected:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	std::string name = "";

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::mat4 model{ 1.0f };

	Object() = default;
	Object(const glm::vec3& position) : position(position)
	{
		update();
	}

	virtual void update()
	{
		model = math::translate(position);
	}
};