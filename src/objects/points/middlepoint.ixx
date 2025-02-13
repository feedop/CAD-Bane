export module middlepoint;

import std;
import glm;

import colors;
import point;
import torus;

export class MiddlePoint : public Point
{
public:
	static MiddlePoint& getInstance()
	{
		static MiddlePoint mp;
		return mp;
	}

	void calculateMiddlePoint(const std::vector<Torus*> selectedTori, const std::vector<Point*> selectedPoints)
	{
		int objectCount = selectedTori.size() + selectedPoints.size();
		if (objectCount < 2)
		{
			isSelected = false;
			return;
		}
		isSelected = true;

		auto sumFunctor = [](const auto& sum, auto object)
		{
			return sum + object->getPosition();
		};
		glm::vec3 sum = std::accumulate(selectedTori.begin(), selectedTori.end(), glm::vec3{0.0f, 0.0f, 0.0f}, sumFunctor)
			+ std::accumulate(selectedPoints.begin(), selectedPoints.end(), glm::vec3{0.0f, 0.0f, 0.0f}, sumFunctor);

		position = 1.0f/ objectCount * sum;
		update();
	}

private:

	MiddlePoint() : Point({ 0.0f, 0.0f, 0.0f })
	{
		color = colors::transparent;
		selectedColor = colors::red;
	}
};