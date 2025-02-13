export module floodfillconfig;

import std;
import glm;

export namespace paths
{
	struct FloodFillConfig
	{
		std::vector<glm::vec2> starts;
		glm::vec3 firstPoint;
		bool millWhite;
		bool byRows;
		float stepBetweenPaths;
		float stepInPath;
		float jumpHeight;
	};
}