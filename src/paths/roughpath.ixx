export module paths:rough;

import std;
import glm;

import scene;
import renderer;
import pathutils;

// Function to find the maximum value within a square of side length `r` centered at (x, y)
float findMaxInSquare(const std::vector<float>& heightMap, int size, int r, int x, int y)
{
	// Calculate half of the side length
	int halfSide = r;

	// Initialize the maximum value to a very low number
	float maxValue = std::numeric_limits<float>::lowest();

	// Iterate over the square area centered at (x, y)
	for (int j = std::max(0, y - halfSide); j <= std::min(size - 1, y + halfSide); ++j)
	{
		for (int i = std::max(0, x - halfSide); i <= std::min(size - 1, x + halfSide); ++i)
		{
			// Calculate the 1D index in the height map
			int index = j * size + i;
			maxValue = std::max(maxValue, heightMap[index]);
		}
	}
	return maxValue;
}

export namespace paths
{
	std::vector<glm::vec3> generateRoughPath(const Scene& scene, const std::vector<float>& heightMap, int heightSize)
	{
		static constexpr float radius = 0.8f;
		static constexpr float eps = radius * 0.12f;
		static constexpr float zLayer1 = 3.3f;
		static constexpr float zLayer2 = 1.6f;
		static constexpr int passes = 10;
		float pointSize = 15.0f / heightSize;
		int radiusInPoints = static_cast<int>(radius / pointSize);

		std::vector<glm::vec3> path;

		// Starting point
		path.emplace_back(0, 0, 5.15f);
		path.emplace_back(-8.5f, -8.0f, 5.15f);
		path.emplace_back(-8.5f, -7.6f + radius - eps, zLayer1);

		// First pass
		for (int i = 0; i < passes; i++)
		{
			float y = -7.5f + radius - eps + i * (2 * radius - eps);
			float startSgn = i % 2 == 0 ? -1.0f : 1.0f;
			path.emplace_back(startSgn * 8.0f, y, zLayer1);

			int yIndex = static_cast<int>((y + 7.5f) * heightSize / 15.0f);
			float prevHeight = zLayer1;
			for (int j = radiusInPoints; j < heightSize - radiusInPoints; j+= radiusInPoints/2)
			{
				int xIndex = i % 2 == 0 ? j : heightSize - radiusInPoints - j - 1;

				float height = findMaxInSquare(heightMap, heightSize, radiusInPoints, xIndex, yIndex);
				if (height > zLayer1 && std::abs(height - prevHeight) >= 1e-5)
				{
					path.emplace_back(xIndex * pointSize - 7.5f, y, height);
					prevHeight = height;
				}	
			}
			path.emplace_back(-startSgn * 8.0f, y, zLayer1);
		}
		{
			auto&& last = path.back();
			path.emplace_back(last.x, last.y, 5.15f);
			path.emplace_back(-8.5f, -8.0f, 5.15f);
		}
		// Second pass
		for (int i = 0; i < passes + 1; i++)
		{
			float y = -7.5f + i * (2 * radius - eps);
			float startSgn = i % 2 == 0 ? -1.0f : 1.0f;
			path.emplace_back(startSgn * 8.0f, y, zLayer2);

			int yIndex = static_cast<int>((y + 7.5f) * heightSize / 15.0f);
			float prevHeight = zLayer2;
			for (int j = radiusInPoints; j < heightSize - radiusInPoints; j += radiusInPoints/2)
			{
				int xIndex = i % 2 == 0 ? j : heightSize - radiusInPoints - j - 1;

				float height = findMaxInSquare(heightMap, heightSize, radiusInPoints, xIndex, yIndex);
				if (height > zLayer2 && std::abs(height - prevHeight) >= 1e-5)
				{
					path.emplace_back(xIndex * pointSize - 7.5f, y, height);
					prevHeight = height;
				}
			}
			path.emplace_back(-startSgn * 8.0f, y, zLayer2);
		}
		{
			// Return to safe position
			auto&& last = path.back();
			path.emplace_back(last.x, last.y, 5.15f);
			path.emplace_back(0, 0, 5.15f);
		}

		// Filter points
		//path = removeCollinearConsecutivePoints(path);

		// Don't mill the base!
		for (auto&& point : path)
		{
			point.z += 0.1f;
		}
		std::vector<float> depths;
		for (auto&& point : path)
			depths.push_back(point.z);
		return path;
	}
}