export module colors;

import <glm/vec4.hpp>;
import <glm/mat4x4.hpp>;

glm::vec4 arrayToVec4(const float* arr)
{
	return glm::vec4(arr[0], arr[1], arr[2], arr[3]);
}

glm::mat4 arrayToMat4(const float* arr)
{
	return glm::mat4(arrayToVec4(arr), arrayToVec4(arr + 4), arrayToVec4(arr + 8), arrayToVec4(arr + 12));
}

inline constexpr float stereoRedData[] = 
{
	1, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 1
};

inline constexpr float stereoBlueData[] =
{
	0, 0, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
	0, 0, 0, 1
};

export namespace colors
{
	inline const glm::vec4 transparent = { 0.0f, 0.0f, 0.0f, 0.0f };
	
	inline const glm::vec4 white = { 1.0f, 1.0f, 0.8f, 1.0f };

	inline const glm::vec4 yellow = { 1.0f, 1.0f, 0.0f, 1.0f };

	inline const glm::vec4 orange = { 1.0f, 0.6f, 0.0f, 1.0f };

	inline const glm::vec4 red =  { 1.0f, 0.0f, 0.0f, 1.0f };

	inline const glm::vec4 navy = { 0.0f, 0.0f, 0.7f, 1.0f };
	
	inline const glm::mat4 stereoRed = glm::transpose(arrayToMat4(stereoRedData));

	inline const glm::mat4 stereoBlue = glm::transpose(arrayToMat4(stereoBlueData));
}