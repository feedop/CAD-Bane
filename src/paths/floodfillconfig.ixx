export module floodfillconfig;

import std;
import glm;

export namespace paths
{
	/// <summary>
	/// Represents the config for flood-filling a single part's parametric space.
	/// </summary>
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