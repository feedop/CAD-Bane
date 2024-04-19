export module colors;

import <glm/vec4.hpp>;

export namespace colors
{
	inline glm::vec4 transparent()
	{
		return { 0.0f, 0.0f, 0.0f, 0.0f };
	}

	inline glm::vec4 white()
	{
		return { 1.0f, 1.0f, 0.8f, 1.0f };
	}

	inline glm::vec4 yellow()
	{
		return { 1.0f, 1.0f, 0.0f, 1.0f };
	}

	inline glm::vec4 orange()
	{
		return { 1.0f, 0.6f, 0.0f, 1.0f };
	}

	inline glm::vec4 red()
	{
		return { 1.0f, 0.0f, 0.0f, 1.0f };
	}

	inline glm::vec4 navy()
	{
		return { 0.0f, 0.0f, 0.7f, 1.0f };
	}
}