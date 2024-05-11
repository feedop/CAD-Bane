export module clickable;

import <glm/vec3.hpp>;

export class Clickable
{
public:
	virtual bool isCoordInObject(const glm::vec3& coord) const = 0;

	virtual ~Clickable() {}
};