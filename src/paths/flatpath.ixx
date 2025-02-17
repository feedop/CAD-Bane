export module paths:flat;

import std;
import glm;

import scene;
import parametric;
import math;
import intersectionconfig;
import floodfillconfig;
import dummysurface;
import intersections;
import surface;
import multitexture;
import depthbmp;
import pathutils;
import concavehull;

using namespace paths;

namespace
{
	void generateFlatPasses(std::vector<glm::vec3>& path, float toolRadius)
	{
		static constexpr float baseLevel = 1.5f;
		const float eps = toolRadius * 0.1f;
		path.emplace_back(-8.5f, -8.6f, 5.15f);
		path.emplace_back(-8.5f, -8.5f, baseLevel);
		path.emplace_back(-8.5f, -7.5f + eps, baseLevel);
		path.emplace_back(7.5f, -7.5f + eps, baseLevel);
		path.emplace_back(7.5f, -7.5f + 2 * toolRadius, baseLevel);
		path.emplace_back(-7.5f + eps, -7.5f + 2 * toolRadius, baseLevel);
		path.emplace_back(-7.5f + eps, -7.5f + 4 * toolRadius, baseLevel);
		path.emplace_back(-4.5f, -7.5f + 4 * toolRadius - eps, baseLevel);
		path.emplace_back(-5.0f, -7.5f + 5.5 * toolRadius - eps, baseLevel);
		path.emplace_back(-7.0f - eps, -7.5f + 4 * toolRadius, baseLevel);
		path.emplace_back(-7.0f - eps, 2.5f, baseLevel);
		path.emplace_back(-6.5f, 0.0f, baseLevel);
		path.emplace_back(-6.5f, -2.0f, baseLevel);
		path.emplace_back(-7.5f, 0.0f, baseLevel);
		path.emplace_back(-7.5f, 7.5f, baseLevel);
		path.emplace_back(-6.5f - eps, 7.5f, baseLevel);
		path.emplace_back(-6.5f - eps, 5.0f, baseLevel);
		path.emplace_back(-5.5f - 2 *eps, 4.5f, baseLevel);
		path.emplace_back(-5.5f - 2 * eps, 7.5f, baseLevel);
		path.emplace_back(-5.5f - 2 * eps, 7.5f, baseLevel);
		path.emplace_back(-5.5f - 2 * eps + toolRadius, 7.5f, baseLevel);
		path.emplace_back(-5.5f - 2 * eps + toolRadius, 4.0f, baseLevel);
		path.emplace_back(-5.5f - 2 * eps + 2 *toolRadius, 7.5f, baseLevel);

		path.emplace_back(7.5f, 7.5f, baseLevel);
		path.emplace_back(7.5f, 6.5f + eps, baseLevel);
		path.emplace_back(-3.2f, 6.5f + eps, baseLevel);
		path.emplace_back(-2.5f, 5.5f + 2 * eps, baseLevel);
		path.emplace_back(7.5f, 5.5f + 2 * eps, baseLevel);
		path.emplace_back(7.5f, 4.5f + 3 * eps, baseLevel);
		path.emplace_back(-1.8f, 4.5f + 3 * eps, baseLevel);
		path.emplace_back(-1.0f, 3.5f + 4 * eps, baseLevel);
		path.emplace_back(7.5f, 3.5f + 4 * eps, baseLevel);
		path.emplace_back(7.5f, 2.5f + 6 * eps, baseLevel);
		path.emplace_back(0.0f, 2.5f + 6 * eps, baseLevel);
		path.emplace_back(2.5f, 2.5f + 6 * eps, baseLevel);
		path.emplace_back(3.7f, 1.5f + 7 * eps, baseLevel);
		path.emplace_back(7.5f, 1.5f + 7 * eps, baseLevel);
		path.emplace_back(7.5f, 0.5f + 8 * eps, baseLevel);
		path.emplace_back(4.4f, 0.5f + 8 * eps, baseLevel);
		path.emplace_back(5.7f, -0.5f + 9 * eps, baseLevel);
		path.emplace_back(7.5f, -0.5f + 9 * eps, baseLevel);
		path.emplace_back(7.5f, -1.5f + 10 * eps, baseLevel);
		path.emplace_back(7.0f + eps, -1.5f + 10 * eps, baseLevel);
		path.emplace_back(6.3f + eps, -1.2f + 10 * eps, baseLevel);
		path.emplace_back(7.0f + eps, -1.5f + 10 * eps, baseLevel);
		path.emplace_back(7.0f + eps, -6.0f, baseLevel);
		path.emplace_back(-3.0f, -6.0f, baseLevel);
		path.emplace_back(-2.0f, -5.0f - eps, baseLevel);
		path.emplace_back(0.0f, -5.0f - eps, baseLevel);
		path.emplace_back(0.0f, -4.5f - eps, baseLevel);
		path.emplace_back(0.0f, -5.0f - eps, baseLevel);
		path.emplace_back(6.5f, -5.0f - eps, baseLevel);
		path.emplace_back(6.5f, -4.5f - eps, baseLevel);
	}

	auto createIntersectionPlan(const std::vector<std::unique_ptr<Surface>>& surfaces, Parametric* base, const glm::vec3& cursorPosition)
	{
		const std::vector<std::tuple<Parametric*, Parametric*, IntersectionConfig>> ret
		{
			{
			surfaces[Body].get(),
			base,
			{
				0.05f,
				false,
				cursorPosition
				}
			},
			{
			surfaces[Body].get(),
			base,
			{
				0.05f,
				true,
				glm::vec3(-1.5797915, 2.8624182, -0.11232458)
				}
			},
			{
			surfaces[LowerCylinder].get(),
			base,
			{
				0.05f,
				true,
				glm::vec3(1.8853462, -3.3570843, 0.042326912)
				}
			},
			{
			surfaces[UpperCylinder].get(),
			base,
			{
				0.05f,
				false,
				cursorPosition
				}
			},
			{
			surfaces[BackFin].get(),
			base,
			{
				0.05f,
				true,
				glm::vec3(-7.010798, -4.7799845, 1.8555614)
				}
			}
		};

		return ret;
	}

	auto createFloodFillPlan(const std::vector<std::unique_ptr<Surface>>& surfaces, Parametric* base, int size)
	{
		std::unordered_map<Parametric*, FloodFillConfig> map
		{
			{ base, { {{size / 2, size / 2 }}, {0, 0, 6}, false} }
		};

		return map;
	}
}

export namespace paths
{
	/// <summary>
	/// Generates a flat path for the given scene. The path is generated with all points at the same height (flat).
	/// This method assumes that the path does not follow the terrain and is simply placed at a constant height level.
	/// </summary>
	/// <param name="scene">The scene object that may contain the necessary context for generating the path.</param>
	/// <returns>A vector of 3D points representing the flat path.</returns>
	std::vector<glm::vec3> generateFlatPath(Scene& scene)
	{
		static constexpr float toolRadius = 0.5f;
		static constexpr int textureSize = 1500;

		auto&& surfaces = scene.getSurfaces();
		std::unique_ptr<Parametric> base = std::make_unique<DummySurface>();
		auto intersectionPlan = createIntersectionPlan(surfaces, base.get(), scene.getCursor()->getPosition());
		auto floodFillPlan = createFloodFillPlan(surfaces, base.get(), textureSize);

		std::vector<glm::vec3> path;

		// Starting point for the border
		path.emplace_back(0, 0, 5.15f);
		path.emplace_back(-8.2f, 0.0f, 5.15f);
		path.emplace_back(-8.4f, 0.0f, 1.5f);

		// Create intersection textures
		std::unordered_map<Parametric*, MultiTexture> textures;
		textures.emplace(base.get(), textureSize);

		std::vector<glm::vec3> fullIntersection;

		// Fill intersection textures
		for (auto&& planDetail : intersectionPlan)
		{
			auto surface1 = std::get<0>(planDetail);
			auto surface2 = std::get<1>(planDetail);
			auto config = std::get<2>(planDetail);

			bool wrap1 = false, wrap2 = false;
			if (auto sur = dynamic_cast<Surface*>(surface1))
			{
				wrap1 = sur->isCylinder();
			}
			if (auto sur = dynamic_cast<Surface*>(surface2))
			{
				wrap2 = sur->isCylinder();
			}
			glm::vec4 wrap = { wrap1, false, wrap2, false };

			auto addPoint = [&scene](const glm::vec3& p)
			{
				// For debugging
			};

			// Find intersection
			auto result = config.useCursor ?
				math::calculateToolDistantIntersection(surface1, surface2, config.d, wrap, toolRadius, 0.0f, addPoint, config.cursorPosition) :
				math::calculateToolDistantIntersection(surface1, surface2, config.d, wrap, toolRadius, 0.0f, addPoint);

			auto&& intersection = std::get<0>(result);
			fullIntersection.insert(fullIntersection.end(), intersection.begin(), intersection.end());

			auto&& params = std::get<1>(result);
			std::cout << "intersection size: " << params.size() << "\n";
			if (params.size() <= 1)
				continue;

			std::vector<glm::vec3> params2;
			std::transform(params.begin(), params.end(), std::back_inserter(params2), [](auto&& pos) { return glm::vec3{ 2.0f * pos.z - 1.0, 2.0f * pos.w - 1.0, 0.0f }; });

			// Fill only the base texture
			auto&& texture2 = textures[surface2];
			texture2.addIntersection(params2, scene.getIntersectionShader(), 1e-3, 1e-3);
		}

		// Add the intersection
		auto hull = getConcaveHullOfFilteredPoints(fullIntersection);

		path.emplace_back(path.back().x, path.back().y, 5.15f);
		path.emplace_back(-8.5f, -3.1f, 5.15f);
		path.emplace_back(-8.2f, -3.1f, 1.5f);
		path.insert(path.end(), hull.begin(), hull.end());

		// Post process: remove points that are too close
		static constexpr float minPointDistance = toolRadius;
		path = removeCloseConsecutivePoints(path, minPointDistance);

		{
			// Return to safe position
			auto&& last = path.back();
			path.emplace_back(last.x, last.y, 5.15f);
			path.emplace_back(0, 0, 5.15f);
		}

		generateFlatPasses(path, toolRadius);
		{
			// Return to safe position
			auto&& last = path.back();
			path.emplace_back(last.x, last.y, 5.15f);
			path.emplace_back(0, 0, 5.15f);
		}

		return path;
	}
}