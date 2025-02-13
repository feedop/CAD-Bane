export module pathutils;

import std;
import glm;

import math;

export namespace paths
{
	enum FishPart
	{
		C0Lower,
		C0Upper,
		LowerMouth,
		UpperMouth,
		Body,
		LowerCylinder,
		UpperCylinder,
		BackFin,
		Base
	};

	struct PathPoint
	{
		glm::vec3 position;
		glm::vec2 uv;
	};

	struct Intersection
	{
		std::vector<glm::vec3> params1;
		std::vector<glm::vec3> params2;
	};

	float pathLength(const std::vector<glm::vec3>& path)
	{
		float len = 0;
		for (int i = 1; i < path.size(); i++)
		{
			len += glm::distance(path[i], path[i - 1]);
		}
		return len;
	}

	std::vector<glm::vec3> removeCloseConsecutivePoints(const std::vector<glm::vec3>& points, float distance)
	{
		std::vector<glm::vec3> result;
		float distanceSquared = distance * distance; // Compare squared distances to avoid sqrt for performance

		if (points.empty()) return result; // Return empty if no points

		// Always keep the first point
		result.push_back(points[0]);

		for (size_t i = 1; i < points.size(); ++i)
		{
			if (glm::any(glm::isnan(points[i])))
				continue;

			//result.push_back(points[i]); // TODO:remove this line
			// Check only the distance to the last added point
			if (glm::distance2(points[i], result.back()) >= distanceSquared)
			{
				result.push_back(points[i]);
			}
		}

		return result;
	}

	// Check if three points are collinear in 2D (ignoring z)
	bool areCollinear(const glm::vec3& prev, const glm::vec3& curr, const glm::vec3& next)
	{
		// Compute the determinant (area of the triangle)
		float area = (curr.x - prev.x) * (next.y - prev.y) - (curr.y - prev.y) * (next.x - prev.x);
		return glm::abs(area) < 1e-6f; // Use a small threshold for floating-point comparisons
	}

	// Filter function to remove collinear points
	std::vector<glm::vec3> removeCollinearConsecutivePoints(const std::vector<glm::vec3>& points)
	{
		if (points.size() < 3) return points; // No filtering needed for less than 3 points

		std::vector<glm::vec3> filtered;
		filtered.push_back(points.front()); // Always keep the first point

		for (size_t i = 1; i < points.size() - 1; ++i)
		{
			const auto& prev = points[i - 1];
			const auto& curr = points[i];
			const auto& next = points[i + 1];

			// Include the current point only if it is not collinear with prev and next
			if (!areCollinear(prev, curr, next))
			{
				filtered.push_back(curr);
			}
		}

		filtered.push_back(points.back()); // Always keep the last point
		return filtered;
	}

	std::vector<glm::vec3> filterPointsByDistance(const std::vector<glm::vec3>& points, float d)
	{
		if (points.empty()) return {};

		std::vector<glm::vec3> filtered;
		bool validSequenceStarted = false;

		for (size_t i = 0; i < points.size() - 1; ++i)
		{
			float distance = glm::distance(points[i], points[i + 1]);

			if (!validSequenceStarted)
			{
				// Find the first pair satisfying the condition
				if (distance <= d)
				{
					filtered.push_back(points[i]);
					validSequenceStarted = true; // Start the sequence
				}
			}
			else
			{
				// Add points as long as the condition is satisfied
				if (distance <= d)
				{
					filtered.push_back(points[i]);
				}
				else
				{
					// Break the loop if a pair violates the condition
					break;
				}
			}
		}
		return filtered;
	}

	std::tuple<std::vector<glm::vec3>, std::vector<glm::vec3>> splitVectorByDistance(const std::vector<glm::vec3>& points, float d)
	{
		size_t splitIndex = points.size(); // Default to no split
		for (size_t i = 0; i < points.size() - 1; ++i)
		{
			if (glm::distance2(points[i], points[i + 1]) > d * d)
			{
				splitIndex = i; // Found the split point
				break;
			}
		}

		// Create the two parts based on the split index
		std::vector<glm::vec3> firstPart(points.begin(), points.begin() + splitIndex + 1);
		std::vector<glm::vec3> secondPart(points.begin() + splitIndex + 1, points.end());

		return { firstPart, secondPart };
	}
}