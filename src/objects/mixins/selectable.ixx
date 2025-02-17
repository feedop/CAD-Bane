export module selectable;

import std;
import glm;

import colors;
import shader;

/// <summary>
/// A base class representing objects that can be selected in the UI. Provides functionality for tracking selection state,
/// setting and getting the object's name, and applying different colors for selected and unselected states.
/// </summary>
export class Selectable
{
public:
	/// <summary>
	/// Indicates whether the object is selected.
	/// </summary>
	bool isSelected = false;

	/// <summary>
	/// Gets the name of the object.
	/// </summary>
	/// <returns>A constant reference to the object's name.</returns>
	inline const std::string& getName() const
	{
		return name;
	}

	/// <summary>
	/// Sets a new name for the object.
	/// </summary>
	/// <param name="newName">The new name to set for the object.</param>
	inline void setName(const std::string& newName)
	{
		name = newName;
	}

	virtual ~Selectable() {}

protected:
	glm::vec4 color = colors::white;
	glm::vec4 selectedColor = colors::yellow;

	/// <summary>
	/// Constructor that initializes the object with a given name.
	/// </summary>
	/// <param name="name">The name to assign to the object.</param>
	Selectable(const std::string& name) : name(name)
	{}

	/// <summary>
	/// Sets the color of the object based on its selection state.
	/// If the object is selected, the selected color is applied; otherwise, the default color is used.
	/// </summary>
	/// <param name="shader">The shader to use for setting the color.</param>
	void setColor(const Shader* shader) const
	{
		shader->setVector("color", isSelected ? selectedColor : color);
	}

private:
	std::string name = "";
};