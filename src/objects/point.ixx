export module point;

import <format>;

import <glm/vec3.hpp>;

import selectableobject;

export class Point : public SelectableObject
{
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
	};

	Point(const glm::vec3& translation = glm::vec3{ 0.0f, 0.0f, 0.0f }) : SelectableObject(translation)
	{
		name = std::format("Point {}", instanceCount++);
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

private:
	friend class GuiController;
	inline static unsigned int instanceCount = 0;
};