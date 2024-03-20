export module selectableobject;

import <string>;

import <glm/vec3.hpp>;

import object;

export class SelectableObject : public Object
{
public:
	bool isSelected = false;

	SelectableObject() = default;
	SelectableObject(const glm::vec3& translation) : Object(translation)
	{}

	virtual void draw(const Shader* shader) const override
	{
		Object::draw(shader);
		shader->setVector("color", isSelected ? selectedColor : color);
	}

	inline const std::string& getName() const
	{
		return name;
	}

	inline void setName(const std::string& newName)
	{
		name = newName;
	}

	virtual bool isCoordInObject(const glm::vec3& coord) const = 0;

protected:
	std::string name = "";
	glm::vec4 color = { 1.0f, 1.0f, 0.8f, 1.0f };
	glm::vec4 selectedColor = { 1.0f, 1.0f, 0.0f, 1.0f };
};