export module vec3conversion;

import <glm/vec3.hpp>;
import <Serializer/Serializer.h>;

export glm::vec3 toGLM(const MG1::Float3& other)
{
	return { other.x, other.y, other.z };
}

export MG1::Float3 toMG1(const glm::vec3& other)
{
	return { other.x, other.y, other.z };
}

void transposePatch(MG1::BezierPatchC2& patch)
{
	std::vector<MG1::PointRef> ret;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.push_back(patch.controlPoints[j * 4 + i]);
		}
	}

	patch.controlPoints = ret;
}

export void transposeSurface(MG1::BezierSurfaceC2& surface)
{
	auto temp = surface.size.x;
	surface.size.x = surface.size.y;
	surface.size.y = temp;
	surface.vWrapped = true;

	for (auto&& patch : surface.patches)
	{
		transposePatch(patch);
	}
}