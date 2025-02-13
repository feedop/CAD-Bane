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

export glm::vec3 endAtEdge(const glm::vec3& pos, float tolU, float tolV)
{
	//static constexpr float tol = 1e-3; //5e-2
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

export void floodFillColor(std::vector<glm::vec4>& image, int x, int y, int size)
{
	auto initialColor = image[toIndex(x, y, size)];
	const glm::vec4 newColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	floodFill(image, x, y, size, initialColor, newColor);
}

export void floodFillBlack(std::vector<float>& image, int x, int y, int size)
{
	if (image.size() == 0)
		return;

	auto initialColor = image[toIndex(x, y, size)];
	static constexpr float newColor = 1.0f;

	floodFill(image, x, y, size, initialColor, newColor);
}