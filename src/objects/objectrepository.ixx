export module objectrepository;

import <memory>;
import <vector>;

import <glm/vec3.hpp>;
import <glm/gtc/constants.hpp>;

import checkergrid;
import cube;
import math;
import object;
import torus;

export class ObjectRepository
{
public:
	ObjectRepository()
	{
		// Initial objects
		tori.emplace_back(new Torus);
	}

	inline const Object* getGrid() const
	{
		return grid.get();
	}

	inline const std::vector<std::unique_ptr<Torus>>& getTori() const
	{
		return tori;
	}

	inline const std::vector<std::unique_ptr<Object>>& getPoints() const
	{
		return points;
	}

	inline void addTorus(const glm::vec3 position)
	{
		tori.emplace_back(new Torus(position));
	}

	void removeTori()
	{
		std::erase_if(tori, [](auto&& torus) { return torus->isSelected; });
	}

private:
	const std::unique_ptr<Object> grid = std::make_unique<CheckerGrid>();

	std::vector<std::unique_ptr<Torus>> tori;
	std::vector<std::unique_ptr<Object>> points;
};