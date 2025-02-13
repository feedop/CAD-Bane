export module intersectionconfig;

import std;
import glm;

export namespace paths
{
	template <int pixelX, int pixelY>
	bool pixelDistanceFilter(const glm::vec3& uv)
	{
		static const glm::vec3 toCompare{ pixelX / 1024.0f, pixelY / 1024.0f, 0.0f };
		auto normalizedUv = (uv + glm::vec3{ 1.0f, 1.0f, 0.0f }) * 0.5f;
		return glm::distance(normalizedUv, toCompare) < 0.01f;
	}

	bool baseUvFilter(const glm::vec3&)
	{
		return false;
	}

	struct IntersectionConfig
	{
		float d;
		bool useCursor;
		glm::vec3 cursorPosition;
		float edgeTolU;
		float edgeTolV;
		std::function<bool(const glm::vec3&)> uvFilter1 = baseUvFilter;
		std::function<bool(const glm::vec3&)> uvFilter2 = baseUvFilter;
	};
}