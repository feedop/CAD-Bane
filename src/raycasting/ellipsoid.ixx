export module ellipsoid;

import glm;

/// <summary>
/// Ellipsoid description
/// </summary>
export struct Ellipsoid
{
	float a;
	float b;
	float c;

	inline static const glm::vec3 color{ 0.7f, 0.7f, 0.0f };
};