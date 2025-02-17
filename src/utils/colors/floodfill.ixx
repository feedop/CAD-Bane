export module floodfill;

import std;
import glm;

inline bool isSkippable(const std::vector<glm::vec3>& positions, int i)
{
	return std::abs(positions[i - 1].x - positions[i].x) > 0.9f ||
		std::abs(positions[i - 1].y - positions[i].y) > 0.9f;
}

inline int toIndex(int x, int y, int size)
{
	return y * size + x;
}

inline bool isValid(int x, int y, int rows, int cols)
{
	return (x >= 0 && x < rows && y >= 0 && y < cols);
}

/// <summary>
/// Performs a flood fill operation to replace all connected pixels of an initial color with a new color.
/// </summary>
/// <typeparam name="T">The type of the image data (e.g., glm::vec4 or float).</typeparam>
/// <param name="image">The image data stored as a linear array.</param>
/// <param name="x">The starting X coordinate.</param>
/// <param name="y">The starting Y coordinate.</param>
/// <param name="size">The size of the image.</param>
/// <param name="initialColor">The initial color to be replaced.</param>
/// <param name="newColor">The new color to apply.</param>
void floodFill(auto& image, int x, int y, int size, auto initialColor, auto newColor)
{
	// Directions array for 4-directional movement (up, down, left, right)
	std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

	// Queue to store the points to process
	std::queue<std::pair<int, int>> q;
	q.push({ x, y });
	image[toIndex(x, y, size)] = newColor; // Set the starting point to the new color

	// BFS loop
	while (!q.empty())
	{
		auto [x, y] = q.front();
		q.pop();

		// Process all 4 neighbors
		for (const auto& dir : directions)
		{
			int newX = x + dir.first;
			int newY = y + dir.second;

			// If the new coordinates are valid and the cell has the initial color
			if (isValid(newX, newY, size, size) && image[toIndex(newX, newY, size)] == initialColor)
			{
				image[toIndex(newX, newY, size)] = newColor;  // Change color
				q.push({ newX, newY });  // Add to queue for further processing
			}
		}
	}
}

/// <summary>
/// Adjusts a position to be slightly outside of the defined boundary if it is near the edge in order to ensure no holes in the curve viewed in the parametric space.
/// </summary>
/// <param name="pos">The input 3D position.</param>
/// <param name="tolU">Tolerance for the X-axis.</param>
/// <param name="tolV">Tolerance for the Y-axis.</param>
/// <returns>The adjusted position.</returns>
export glm::vec3 endAtEdge(const glm::vec3& pos, float tolU, float tolV)
{
	glm::vec3 newPos = pos;
	if (newPos.x < -1.0f + tolU)
		newPos.x = -1.2f;
	else if (newPos.x > 1.0f - tolU)
		newPos.x = 1.2f;

	if (newPos.y < -1.0f + tolV)
		newPos.y = -1.2f;
	else if (newPos.y > 1.0f - tolV)
		newPos.y = 1.2f;

	return newPos;
}

// <summary>
/// Modifies a list of positions to adapt them based on edge tolerance.
/// </summary>
/// <param name="positions">The input vector of 3D positions.</param>
/// <param name="tolU">Tolerance for X-axis.</param>
/// <param name="tolV">Tolerance for Y-axis.</param>
/// <returns>A vector of adjusted positions.</returns>
export std::vector<glm::vec3> adaptPositions(const std::vector<glm::vec3>& positions, float tolU = 1e-3, float tolV = 1e-3)
{
	std::vector<glm::vec3> ret;
	for (int i = 1; i < positions.size(); i++)
	{
		if (isSkippable(positions, i))
			continue;

		ret.push_back(endAtEdge(positions[i - 1], tolU, tolV));
		ret.push_back(endAtEdge(positions[i], tolU, tolV));
	}
	return ret;
}

/// <summary>
/// Performs a flood fill on an image represented as a vector of RGBA colors.
/// </summary>
/// <param name="image">The image data stored as a vector of glm::vec4.</param>
/// <param name="x">The starting X coordinate.</param>
/// <param name="y">The starting Y coordinate.</param>
/// <param name="size">The size of the image.</param>
export void floodFillColor(std::vector<glm::vec4>& image, int x, int y, int size)
{
	auto initialColor = image[toIndex(x, y, size)];
	const glm::vec4 newColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	floodFill(image, x, y, size, initialColor, newColor);
}

/// <summary>
/// Performs a flood fill on an image represented as a vector of grayscale values.
/// </summary>
/// <param name="image">The image data stored as a vector of floats.</param>
/// <param name="x">The starting X coordinate.</param>
/// <param name="y">The starting Y coordinate.</param>
/// <param name="size">The size of the image.</param>
export void floodFillBlack(std::vector<float>& image, int x, int y, int size)
{
	if (image.size() == 0)
		return;

	auto initialColor = image[toIndex(x, y, size)];
	static constexpr float newColor = 1.0f;

	floodFill(image, x, y, size, initialColor, newColor);
}