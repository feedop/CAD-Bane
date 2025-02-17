export module middlepoint;

import std;
import glm;

import colors;
import point;
import torus;

/// <summary>
/// Represents a singleton object that calculates the middle point between selected objects, such as tori and points.
/// </summary>
export class MiddlePoint : public Point
{
public:
	/// <summary>
	/// Retrieves the singleton instance of the MiddlePoint class.
	/// </summary>
	/// <returns>The singleton instance of the MiddlePoint.</returns>
	static MiddlePoint& getInstance()
	{
		static MiddlePoint mp;
		return mp;
	}

	/// <summary>
	/// Calculates the middle point of a set of selected tori and points.
	/// The middle point is calculated as the average of the positions of all selected objects.
	/// </summary>
	/// <param name="selectedTori">A vector of selected tori objects to be considered in the middle point calculation.</param>
	/// <param name="selectedPoints">A vector of selected point objects to be considered in the middle point calculation.</param>
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

	// <summary>
	/// Private constructor for the MiddlePoint singleton instance. Initializes the middle point's position and color.
	/// </summary>
	MiddlePoint() : Point({ 0.0f, 0.0f, 0.0f })
	{
		color = colors::transparent;
		selectedColor = colors::red;
	}
};