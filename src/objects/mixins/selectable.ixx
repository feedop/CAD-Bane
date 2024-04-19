export module selectable;

import <string>;

import <glm/vec4.hpp>;

import colors;
import shader;

export class Selectable
{
public:
	bool isSelected = false;

	inline const std::string& getName() const
	{
		return name;
	}

	inline void setName(const std::string& newName)
	{
		name = newName;
	}

protected:
	glm::vec4 color = colors::white();
	glm::vec4 selectedColor = colors::yellow();

	Selectable(const std::string& name) : name(name)
	{}

	void setColor(const Shader* shader) const
	{
		shader->setVector("color", isSelected ? selectedColor : color);
	}

private:
	std::string name = "";
};