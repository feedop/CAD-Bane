export module pathutils;

import std;
import glm;

import math;

export namespace paths
{
	/// <summary>
	/// Defines parts of the fish model.
	/// </summary>
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

	/// <summary>
	/// Represents a single point on the path.
	/// </summary>
	struct PathPoint
	{
		glm::vec3 position;
		glm::vec2 uv;
	};

	/// <summary>
	/// Represents an intersection curve represented in the parameter space of both surfaces.
	/// </summary>
	struct Intersection
	{
		std::vector<glm::vec3> params1;
		std::vector<glm::vec3> params2;
	};

	/// <summary>
	/// Computes the total length of a given path by summing the distances between consecutive points.
	/// </summary>
	/// <param name="path">A vector of 3D points representing the path.</param>
	/// <returns>The total length of the path.</returns>
	float pathLength(const std::vector<glm::vec3>& path)
	{
		float len = 0;
		for (int i = 1; i < path.size(); i++)
		{
			len += glm::distance(path[i], path[i - 1]);
		}
		return len;
	}

	/// <summary>
	/// Removes consecutive points that are closer than a specified distance to each other.
	/// </summary>
	/// <param name="points">A vector of 3D points.</param>
	/// <param name="distance">The minimum allowed distance between consecutive points.</param>
	/// <returns>A vector of points with close consecutive points removed.</returns>
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

	/// <summary>
	/// Checks if three consecutive points are collinear in 2D (ignoring the Z component).
	/// </summary>
	/// <param name="prev">The first point.</param>
	/// <param name="curr">The second point.</param>
	/// <param name="next">The third point.</param>
	/// <returns>True if the points are collinear, false otherwise.</returns>
	bool areCollinear(const glm::vec3& prev, const glm::vec3& curr, const glm::vec3& next)
	{
		// Compute the determinant (area of the triangle)
		float area = (curr.x - prev.x) * (next.y - prev.y) - (curr.y - prev.y) * (next.x - prev.x);
		return glm::abs(area) < 1e-6f; // Use a small threshold for floating-point comparisons
	}

	/// <summary>
	/// Removes consecutive collinear points from a given set of points.
	/// </summary>
	/// <param name="points">A vector of 3D points.</param>
	/// <returns>A vector of points with collinear points removed.</returns>
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

	/// <summary>
	/// Filters a sequence of points by distance, keeping only consecutive points where the 
	/// distance between them is less than or equal to a given threshold.
	/// </summary>
	/// <param name="points">A vector of 3D points to be filtered.</param>
	/// <param name="d">The maximum allowed distance between consecutive points.</param>
	/// <returns>
	/// A vector containing the longest sequence of consecutive points where the distance condition is met.
	/// </returns>
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

	/// <summary>
	/// Splits a vector of 3D points into two parts based on a distance threshold.
	/// The split occurs at the first point where the distance condition is violated.
	/// </summary>
	/// <param name="points">A vector of 3D points to be split.</param>
	/// <param name="d">The maximum allowed distance between consecutive points before a split occurs.</param>
	/// <returns>
	/// A tuple containing two vectors:
	/// - The first part includes points up to the split index (inclusive).
	/// - The second part includes points after the split index.
	/// </returns>
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