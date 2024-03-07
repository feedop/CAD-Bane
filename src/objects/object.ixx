export module object;

import <glm/mat4x4.hpp>;

import shader;

export struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
};

export class Object
{
public:
	virtual void draw(const Shader* shader) const
	{
		shader->setMatrix("model", model);
	}

	virtual ~Object() = default;

protected:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	glm::mat4 model{ 1.0f };

	Object() = default;
	Object(const glm::mat4& model) : model(model)
	{}
};