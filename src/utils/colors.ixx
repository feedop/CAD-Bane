export module colors;

import <glm/vec4.hpp>;

export namespace colors
{
	inline const glm::vec4 transparent = { 0.0f, 0.0f, 0.0f, 0.0f };
	
	inline const glm::vec4 white = { 1.0f, 1.0f, 0.8f, 1.0f };

	inline const glm::vec4 yellow = { 1.0f, 1.0f, 0.0f, 1.0f };

	inline const glm::vec4 orange = { 1.0f, 0.6f, 0.0f, 1.0f };

	inline const glm::vec4 red =  { 1.0f, 0.0f, 0.0f, 1.0f };

	inline const glm::vec4 navy = { 0.0f, 0.0f, 0.7f, 1.0f };
}