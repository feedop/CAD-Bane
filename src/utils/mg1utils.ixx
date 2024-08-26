export module mg1utils;

import <glm/vec3.hpp>;
import <Serializer/Serializer.h>;

class Point;

export glm::vec3 toGLM(const MG1::Float3& other)
{
	return { other.x, other.y, other.z };
}

export MG1::Float3 toMG1(const glm::vec3& other)
{
	return { other.x, other.y, other.z };
}

void transposePatch(auto& patch)
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

export void transposeC0(MG1::BezierSurfaceC0& surface)
{
	std::vector<MG1::BezierPatchC0> tempPatches;

	for (int i = 0; i < surface.size.x; i++)
	{
		for (int j = 0; j < surface.size.y; j++)
		{
			tempPatches.emplace_back((std::move(surface.patches[j * surface.size.x + i])));
		}
	}

	surface.patches = std::move(tempPatches);

	for (auto&& patch : surface.patches)
	{
		transposePatch(patch);
	}
}

export template <typename T>
requires(std::is_same_v<T, MG1::BezierSurfaceC0> || std::is_same_v<T, MG1::BezierSurfaceC2>)
void transposeSurface(T& surface)
{
	using PatchType = std::conditional_t<std::is_same_v<T, MG1::BezierSurfaceC0>, MG1::BezierPatchC0, MG1::BezierPatchC2>;

	auto temp1 = surface.size.x;
	surface.size.x = surface.size.y;
	surface.size.y = temp1;
	auto temp2 = surface.uWrapped;
	surface.uWrapped = surface.vWrapped;
	surface.vWrapped = temp2;

	std::vector<PatchType> tempPatches;

	for (int i = 0; i < surface.size.y; i++)
	{
		for (int j = 0; j < surface.size.x; j++)
		{
			tempPatches.emplace_back((std::move(surface.patches[j  * surface.size.y + i])));
		}
	}

	surface.patches = std::move(tempPatches);

	for (auto&& patch : surface.patches)
	{
		transposePatch(patch);
	}
}



export auto findPoint(const auto& allPoints, unsigned int id)
{
	return std::find_if(allPoints.begin(), allPoints.end(), [&](auto& p) { return p->getId() == id; })->get();
}