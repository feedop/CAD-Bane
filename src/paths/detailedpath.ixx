export module paths:detailed;

import std;
import glm;

import config;
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
	auto createIntersectionPlan(const std::vector<std::unique_ptr<Surface>>& surfaces, Parametric* base, const glm::vec3& cursorPosition)
	{
		const std::vector<std::tuple<Parametric*, Parametric*, IntersectionConfig>> ret
		{
			{
			surfaces[BackFin].get(),
			base,
			{
				0.05f,
				false,
				cursorPosition,
				1e-2,
				1e-2
				}
			},
			{
			surfaces[BackFin].get(),
			surfaces[Body].get(),
			{
				0.05f,
				true,
				glm::vec3(-6.608512, 4.1850295, 1.1716632),
				1e-2,
				1e-2
				}
			},
			{ // Two intersections for body-base
			surfaces[Body].get(),
			base,
			{
				0.05f,
				true,
				glm::vec3(-2.2119844, 3.6405194, 1.5328687),
				1e-2,
				1e-2,
				pixelDistanceFilter<976, 717>,
				baseUvFilter
				}
			},
			{
			surfaces[Body].get(),
			base,
			{
				0.05f,
				true,
				glm::vec3(-3.6305225, -2.7461474, 3.3968654),
				1e-2,
				1e-2,
				pixelDistanceFilter<442, 274>,
				baseUvFilter,
				}
			},
			{
			surfaces[UpperCylinder].get(),
			surfaces[Body].get(),
			{
				0.05f,
				true,
				glm::vec3(0.93804204, 0.94176376, 2.6582785),
				5e-2,
				1e-2,
				pixelDistanceFilter<185, 225>,
				baseUvFilter,
				}
			},
			{
			surfaces[LowerCylinder].get(),
			surfaces[Body].get(),
			{
				0.05f,
				true,
				glm::vec3(1.536875, 0.8982603, 3.3812091),
				1e-2,
				1e-2
				}
			},
			{ // Two intersections for uppercylinder-base
			surfaces[UpperCylinder].get(), // (outside)
			base,
			{
				0.05f,
				false,
				cursorPosition,
				1e-2,
				1e-2,
				pixelDistanceFilter<814, 313>,
				baseUvFilter,
				},
			},
			{ // (inside)
			surfaces[UpperCylinder].get(),
			base,
			{
				0.05f,
				true,
				glm::vec3(1.9493794, -0.17481863, 1.582583),
				1e-2,
				1e-2,
				pixelDistanceFilter<301, 490>,
				pixelDistanceFilter<660, 550>,
				}
			},
			{ // Two intersections for uppercylinder-lowercylinder
			surfaces[UpperCylinder].get(),
			surfaces[LowerCylinder].get(),
			{
				0.05f,
				false,
				cursorPosition,
				1e-2,
				1e-2
				}
			},
			{
			surfaces[UpperCylinder].get(),
			surfaces[LowerCylinder].get(),
			{
				0.05f,
				true,
				glm::vec3(4.420517, 0.08728799, 1.4297256),
				3e-2,
				1e-2
				}
			},
			// Two intersections for lowercylinder-base 
			{ //(inside)
			surfaces[LowerCylinder].get(),
			base,
			{
				0.05f,
				true,
				glm::vec3(4.165834, -1.3204753, 1.216224),
				1e-2,
				5e-2,
				pixelDistanceFilter<924,892>,
				pixelDistanceFilter<801,423>	
				}
			},
			{ //(outside)
			surfaces[LowerCylinder].get(),
			base,
			{
				0.05f,
				true,
				glm::vec3(1.7023045, -4.464113, 1.8604698),
				1e-2,
				3e-2,
				pixelDistanceFilter<370,355>	,
				baseUvFilter
				}
			},
			{ // C0 surfaces with base
			surfaces[Body].get(),
			surfaces[C0Lower].get(),
			{
				0.02f,
				false,
				cursorPosition,
				1e-2,
				1.99e-2,
				pixelDistanceFilter<181, 855>,
				pixelDistanceFilter<820, 928>
				}
			},
			{ // C0 surfaces with base
			surfaces[Body].get(),
			surfaces[C0Upper].get(),
			{
				0.05f,
				false,
				cursorPosition,
				1e-2,
				2e-2,
				pixelDistanceFilter<175, 712>,
				[](auto&& uv)
				{
					return uv.x < 0.0f && uv.y < 0.0f;
				}
				}
			},
		};

		return ret;
	}

	auto createFloodFillPlan(const std::vector<std::unique_ptr<Surface>>& surfaces, Parametric* base, int size)
	{
		std::unordered_map<Parametric*, FloodFillConfig> map
		{
			{ surfaces[Body].get(), { {{1, size / 2 }, {size-1, size / 2 }}, {-5, 5, 5.15}, true, false, 0.0035f, 0.01f, 4.65f} },
			{ surfaces[LowerCylinder].get(), { {{1, size / 2}, { size - 1, size / 2 } }, {0, 2, 5.15}, true, false, 0.0069f, 0.01f, 2.3f} },
			{ surfaces[UpperCylinder].get(), { {{1, size / 2}, { size - 1, size / 2 } }, {0, 0, 5.15}, true, false, 0.0087f, 0.01f, 2.5f} },
			{ surfaces[BackFin].get(), { {{size * 0.65f, size / 2 }}, {-6, 0, 5.3}, true, true, 0.009f, 0.01f, 4.65f} },
			{ surfaces[C0Lower].get(), { {{size / 2, size / 2} }, {0, -4, 5.3}, true, false, 0.03f, 0.005f, 4.65f} },
			{ surfaces[C0Upper].get(), { {{size / 2, size / 2}}, {0, 4, 5.3}, true, false, 0.03f, 0.005f, 4.65f} },
			{ base, { {{static_cast<int>(size * 0.67), static_cast<int>(size * 0.45)}}, {1.27, 0, 5.15}, true, false, 0.0045f, 0.005f, 4.65f}}
		};

		return map;
	}
}

export namespace paths
{
	std::vector<glm::vec3> generateDetailedPath(Scene& scene)
	{
		static constexpr float toolRadius = 0.4f;
		static constexpr int textureSize = 1024;

		auto&& surfaces = scene.getSurfaces();
		std::unique_ptr<Parametric> base = std::make_unique<DummySurface>();
		auto intersectionPlan = createIntersectionPlan(surfaces, base.get(), scene.getCursor()->getPosition());
		auto floodFillPlan = createFloodFillPlan(surfaces, base.get(), textureSize);

		std::vector<glm::vec3> path;
		std::vector<Intersection> intersections;

		// Starting point
		path.emplace_back(0, 0, 5.15f);

		// Create intersection textures
		std::unordered_map<Parametric*, MultiTexture> textures;
		for (auto& surface : surfaces)
		{
			textures.emplace(surface.get(), textureSize);
		}
		textures.emplace(base.get(), textureSize);

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
				//scene.addPoint(p);
			};
			
			// Find intersection
			auto result = config.useCursor ?
				math::calculateToolDistantIntersection(surface1, surface2, config.d, wrap, toolRadius, addPoint, config.cursorPosition) :
				math::calculateToolDistantIntersection(surface1, surface2, config.d, wrap, toolRadius, addPoint);

			for (auto&& point : std::get<0>(result)) // debug 2137
			{
				//scene.addPoint(point);
			}	
			
			auto&& params = std::get<1>(result);
			std::cout << "intersection size: " << params.size() << "\n";

			std::vector<glm::vec3> params1;
			std::transform(params.begin(), params.end(), std::back_inserter(params1), [](auto&& pos) { return glm::vec3{ 2.0f * pos.x - 1.0f, 2.0f * pos.y - 1.0, 0.0f }; });
			std::vector<glm::vec3> params2;
			std::transform(params.begin(), params.end(), std::back_inserter(params2), [](auto&& pos) { return glm::vec3{ 2.0f * pos.z - 1.0, 2.0f * pos.w - 1.0, 0.0f }; });

			auto&& texture1 = textures[surface1];
			auto&& texture2 = textures[surface2];

			// Apply filters
			params1.erase(std::remove_if(params1.begin(), params1.end(), config.uvFilter1), params1.end());
			params2.erase(std::remove_if(params2.begin(), params2.end(), config.uvFilter2), params2.end());
			
			if (surface2 == surfaces[C0Lower].get())
			{
				params1.emplace(params1.begin(), 2.0f * (155.0f / 1024.0f) - 1.0f, 2.0f * (883.0f / 1024.0f) - 1.0f, 0.0f);
				params1.emplace_back(2.0f * (189.0f / 1024.0f) - 1.0f, 2.0f * (620.0f / 1024.0f) - 1.0f, 0.0f);

				params2.emplace_back(2.0f * (30.0f / 1024.0f) - 1.0f, -1.0f, 0);
			}
			else if (surface2 == surfaces[C0Upper].get())
			{
				params1.emplace(params1.begin(), 2.0f * (189.0f / 1024.0f) - 1.0f, 2.0f * (620.0f / 1024.0f) - 1.0f, 0.0f);
				params1.emplace_back(2.0f * (155.0f / 1024.0f) - 1.0f, 2.0f * (883.0f / 1024.0f) - 1.0f, 0.0f);

				params2.emplace(params2.begin(), 2.0f * (930.0f / 1024.0f) - 1.0f, -1.0f, 0);
			}

			// Add to texture	
			texture1.addIntersection(params1, scene.getIntersectionShader(), config.edgeTolU, config.edgeTolV);

			if (surface1 == surfaces[UpperCylinder].get() && surface2 == surfaces[Body].get())
			{
				auto [p21, p22] = splitVectorByDistance(params2, 0.3f);
				p21.emplace_back(-1.0f, 2.0f * (980.0f / 1024.0f) - 1.0f, 0.0f);

				texture2.addIntersection(p21, scene.getIntersectionShader(), 1e-4, 1e-4);
				texture2.addIntersection(p22, scene.getIntersectionShader(), config.edgeTolU, config.edgeTolV);
			}
			else
			{
				texture2.addIntersection(params2, scene.getIntersectionShader(), config.edgeTolU, config.edgeTolV);
			}			

			intersections.emplace_back(params1, params2);
		}

		// Flood fill the textures
		auto addPartToPath = [&](FishPart part, bool swapUV = false)
		{
			auto&& surface = part == Base ? base.get() : surfaces[part].get();
			auto&& texture = textures[surface];
			auto&& floodFillConfig = floodFillPlan[surface];
			bool millWhite = floodFillConfig.millWhite;
			bool byRows = floodFillConfig.byRows;
			if (swapUV)
				byRows = !byRows;

			if (!swapUV)
			{
				static constexpr bool dummy = false; // DUMMY = ?
				for (auto&& start : floodFillConfig.starts)
				{
					texture.floodFill(start.x, start.y, dummy);
				}

				// Remove unnecessary edges
				if (!dummy)
					texture.imopen();

				auto&& data = texture.getData();

				// Optional: save to file
				if constexpr (cfg::savePathTextures)
					saveDepthBmp(texture.getData(), textureSize, textureSize, std::format("paths/intersections{}.bmp", static_cast<int>(part)).c_str());
			}		

			// Based on textures create the paths
			float stepU = floodFillConfig.stepBetweenPaths;
			const float stepV = floodFillConfig.stepInPath;
			const float millColor = millWhite ? 1.0f : 0.0f;

			PathPoint prevPoint{
				{0,0,0},
				{-1,-1}
			};

			auto checkPoint = [&](float u, float v)
			{
				if (byRows)
					std::swap(u, v);

				if (part == C0Lower && ((v < 0.15f && u < 0.2f) || (v > 0.92f && u < 0.1f)))
					return;

				if (part == C0Upper && v > 0.9f && u > 0.9f)
					return;

				if (part == Body && v > 0.88f && u > 0.24f && u < 0.7f)
					return;

				if (part == Body && v > 0.89f && u > 0.16f && u <= 0.24f)
					return;

				if (part == Body && v > 0.9f && u > 0.09f && u <= 0.11f)
					return;

				if (part == Body && v > 0.85f && u <= 0.002f)
					return;

				if (part == UpperCylinder && v < 0.17f && u > 0.95f)
					return;

				auto pos = surface->evaluate(u, v, toolRadius);
				float color = texture.sample(u, v);
				if (color == millColor)
				{
					PathPoint thisPoint{
						pos,
						{u, v}
					};

					if (prevPoint.uv != glm::vec2{-1, -1} && glm::length(thisPoint.uv - prevPoint.uv) > 1 * stepU && glm::length(thisPoint.position - prevPoint.position) > 1 * toolRadius)
					{
						// Add two intermediate points to jump over a hole
						float jumpHeight = floodFillConfig.jumpHeight;
						float vectorModifier = 0.2f;
						auto dist = thisPoint.position - prevPoint.position;

						if (part == Body && glm::length(dist) < 5.0f && thisPoint.position.z < 2.5f)
							jumpHeight = 2.5f;

						if (part == Body && thisPoint.position.z < 2.5f && u < 0.1f)
							vectorModifier = 0.1f;

						if (part == LowerCylinder && glm::length(dist) > 4.5f)
							vectorModifier = 0.1f;

						path.push_back(glm::vec3{ prevPoint.position.x, prevPoint.position.y, jumpHeight } + vectorModifier * dist);
						path.push_back(glm::vec3{ thisPoint.position.x, thisPoint.position.y, jumpHeight } - vectorModifier * dist);
					}

					path.push_back(pos); // debug 2137

					prevPoint = thisPoint;
				}
			};

			path.push_back(floodFillConfig.firstPoint);

			float uStartModifier = 0.0f;
			float uEndModifier = 0.0f;
			float vStartModifier = 0.0f;
			float vEndModifier = 0.0f;

			if (part == C0Lower)
			{
				uStartModifier = 0.00f;
				vEndModifier = 0.0f;
			}			

			bool forward = true;
			for (float u = uStartModifier; u <= 1.0f - uEndModifier; u += stepU)
			{
				if (forward)
				{
					// Iterate v forward
					for (float v = vStartModifier; v <= 1.0f - vEndModifier; v += stepV)
					{
						checkPoint(u, v);
					}
				}
				else
				{
					// Iterate v backward
					for (float v = 1.0f - vEndModifier; v >= vStartModifier; v -= stepV)
					{
						checkPoint(u, v);
					}
				}
				forward = !forward;

				if (part == Body) // Adjust step between paths for different parts
				{
					if (u < 0.025f || (u <= 0.7f && u > 0.38f))
						stepU = 0.6f * floodFillConfig.stepBetweenPaths;
					else if (u > 0.7f)
						stepU = 0.35f * floodFillConfig.stepBetweenPaths;
					else
						stepU = floodFillConfig.stepBetweenPaths;
				}

				if (part == BackFin) // Adjust step between paths for different parts
				{
					if (u > 0.38f && u < 0.62f)
						stepU = 0.5f * floodFillConfig.stepBetweenPaths;
					else if (u >= 0.62)
						stepU = 1.2 * floodFillConfig.stepBetweenPaths;
				}
			}

			if (part == Body)
			{
				path.resize(path.size() - 66);
			}

			auto&& last = path.back();
			path.emplace_back(last.x, last.y, 5.15f);
			path.emplace_back(0, 0, 5.15f);
		};

		// Add parts to the final path
		addPartToPath(Body);
		addPartToPath(C0Lower);
		addPartToPath(C0Upper);
		addPartToPath(BackFin);
		addPartToPath(UpperCylinder);
		addPartToPath(LowerCylinder);	
		addPartToPath(Base);
		addPartToPath(Base, true);

		// Add intersections to the final path
		for (int i = 0; i < intersections.size(); i++)
		{
			auto&& intersection = intersections[i];
			auto&& planDetail = intersectionPlan[i];
			auto&& surface1 = std::get<0>(planDetail);
			auto&& surface2 = std::get<1>(planDetail);
			float millColor1 = floodFillPlan[surface1].millWhite ? 1.0f : 0.0f;
			float millColor2 = floodFillPlan[surface2].millWhite ? 1.0f : 0.0f;
			auto&& params1 = intersection.params1;
			auto&& params2 = intersection.params2;

			// Skip intersections with base - already covered by the flat path
			if (surface2 == base.get() && i != 7 && i != 10) // Check the inside intersections for the eye
				continue;

			if (surface1 == surfaces[BackFin].get())
				std::reverse(params1.begin(), params1.end());

			std::vector<glm::vec3> intersectionPath;
			for (int j = 0; j < params1.size(); j++)
			{
				auto uv1 = (params1[j] + 1.0f) * 0.5f;
				//auto uv2 = (params2[j] + 1.0f) * 0.5f;
				// Check whether this intersection point is millColor on the both surfaces
				float color1 = textures[surface1].sample(uv1.x, uv1.y);
				//float color2 = textures[surface2].sample(uv2.x, uv2.y);
				if (color1 == millColor1)
				{
					auto pos = surface1->evaluate(uv1.x, uv1.y, toolRadius);
					intersectionPath.push_back(pos);
					//scene.addPoint(intersectionPath.back()); //2137 debug			
				}				
			}

			if (intersectionPath.size() == 0)
				continue;
			intersectionPath = filterPointsByDistance(intersectionPath, 0.7f);
			auto&& last = intersectionPath.back();
			intersectionPath.emplace_back(last.x, last.y, 5.15f);
			auto&& firstInIntersection = intersectionPath.front();

			path.emplace_back(0, 0, 5.15f);
			path.emplace_back(firstInIntersection.x + 0.1f, firstInIntersection.y, 5.15f);
			path.insert(path.end(), std::make_move_iterator(intersectionPath.begin()), std::make_move_iterator(intersectionPath.end()));
		}

		// Post process: remove points that are too close
		static constexpr float minPointDistance = 0.1f * toolRadius;
		path = removeCloseConsecutivePoints(path, minPointDistance);

		// Account for the fact that paths are expressed as the position of the mill's tip, not center
		for (auto&& pos : path)
		{
			pos = glm::vec3{pos.x, pos.y, std::max(pos.z - toolRadius, 1.5f)};
		}

		{
			// Return to safe position
			auto&& last = path.back();
			path.emplace_back(last.x, last.y, 5.15f);
			path.emplace_back(0, 0, 5.15f);
		}

		return path;
	}
}