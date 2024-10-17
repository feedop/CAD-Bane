export module c0surface;

import std;

import <glm/vec3.hpp>;
import <Serializer/Serializer.h>;

import gregorystructs;
import math;
import patch;
import surface;
import mg1utils;

using namespace Gregory;

export class C0Surface : public Surface
{
public:
	C0Surface(const glm::vec3& position, int sizeX, int sizeZ) : Surface(getSurfaceName(), sizeX, sizeZ)
	{
		std::vector<Point*> tempPoints;

		// generate points
		// first patch in first row
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				points.emplace_back(new Point(position + glm::vec3{patchSizeX * i / 3.0f, 0, 0 } + glm::vec3{0, 0, patchSizeZ * j / 3.0f}));
				tempPoints.push_back(points[points.size() - 1]);
			}	
		}
		patches.emplace_back(new Patch(tempPoints));

		// the rest of the first row
		for (int c = 1; c < sizeX; c++)
		{
			tempPoints.erase(tempPoints.begin(), tempPoints.begin() + 12);
			glm::vec3 offset = position + glm::vec3{ patchSizeX * c, 0, 0 };
			for (int i = 1; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					points.emplace_back(new Point(offset + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + glm::vec3{ 0, 0, patchSizeZ * j / 3.0f }));
					tempPoints.push_back(points[points.size() - 1]);
				}
			}
			patches.emplace_back(new Patch(tempPoints));
		}

		tempPoints.clear();
		// the next rows
		for (int r = 1; r < sizeZ; r++)
		{
			glm::vec3 rowOffset = position + glm::vec3{ 0, 0, patchSizeZ * r };

			// first patch in row
			for (int i = 0; i < 4; i++)
			{
				if (r == 1)
					tempPoints.push_back(points[i * 4 + 3]);
				else
					tempPoints.push_back(points[16 + (sizeX - 1) * 12 + (12 + 9 * (sizeX - 1)) * (r - 2) + i * 3 + 2]); // magic
				for (int j = 1; j < 4; j++)
				{
					points.emplace_back(new Point(rowOffset + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + glm::vec3{ 0, 0, patchSizeZ * j / 3.0f }));
					tempPoints.push_back(points[points.size() - 1]);
				}
			}
			patches.emplace_back(new Patch(tempPoints));

			// the rest of the row
			for (int c = 1; c < sizeX; c++)
			{
				tempPoints.erase(tempPoints.begin(), tempPoints.begin() + 12);
				glm::vec3 offset = rowOffset + glm::vec3{ patchSizeX * c, 0, 0 };
				for (int i = 1; i < 4; i++)
				{
					if (r == 1)
						tempPoints.push_back(points[16 + (c - 1) * 12 + (i - 1) * 4 + 3]);
					else
						tempPoints.push_back(points[12 + (c - 1) * 9 + 16 + (sizeX - 1) * 12 + (12 + 9 * (sizeX - 1)) * (r - 2) + (i - 1) * 3 + 2]); // even more magic
					for (int j = 1; j < 4; j++)
					{
						points.emplace_back(new Point(offset + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + glm::vec3{ 0, 0, patchSizeZ * j / 3.0f }));
						tempPoints.push_back(points[points.size() - 1]);
					}
				}
				patches.emplace_back(new Patch(tempPoints));
			}
			tempPoints.clear();
		}

		attachPoints();
	}

	C0Surface(const glm::vec3& position, int sizeX, float radius) : Surface(getSurfaceName(), sizeX, std::max(3, static_cast<int>(2 * math::pi * radius / patchSizeZ)), true)
	{
		int pointCountZ = 4 + 3 * (sizeZ - 2) + 2;
		float minAngleDist = 2 * math::pi / pointCountZ;

		std::vector<Point*> tempPoints;

		// generate points
		// first patch in first row
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				float angle = minAngleDist * j;
				points.emplace_back(new Point(position + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + radius * glm::vec3{ 0, std::cos(angle), std::sin(angle)}));
				tempPoints.push_back(points[points.size() - 1]);
			}
		}
		patches.emplace_back(new Patch(tempPoints));

		// the rest of the first row
		for (int c = 1; c < sizeX; c++)
		{
			tempPoints.erase(tempPoints.begin(), tempPoints.begin() + 12);
			glm::vec3 offset = position + glm::vec3{ patchSizeX * c, 0, 0 };
			for (int i = 1; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					float angle = minAngleDist * j;
					points.emplace_back(new Point(offset + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + radius * glm::vec3{ 0, std::cos(angle), std::sin(angle) }));
					tempPoints.push_back(points[points.size() - 1]);
				}
			}
			patches.emplace_back(new Patch(tempPoints));
		}

		tempPoints.clear();
		// the next rows
		for (int r = 1; r < sizeZ - 1; r++)
		{
			// first patch in row
			for (int i = 0; i < 4; i++)
			{
				if (r == 1)
					tempPoints.push_back(points[i * 4 + 3]);
				else
					tempPoints.push_back(points[16 + (sizeX - 1) * 12 + (12 + 9 * (sizeX - 1)) * (r - 2) + i * 3 + 2]); // magic
				for (int j = 1; j < 4; j++)
				{
					float angle = minAngleDist * (4 + 3 * (r - 1) + j - 1);
					points.emplace_back(new Point(position + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + radius * glm::vec3{ 0, std::cos(angle), std::sin(angle) }));
					tempPoints.push_back(points[points.size() - 1]);
				}
			}
			patches.emplace_back(new Patch(tempPoints));

			// the rest of the row
			for (int c = 1; c < sizeX; c++)
			{
				tempPoints.erase(tempPoints.begin(), tempPoints.begin() + 12);
				glm::vec3 offset = position + glm::vec3{ patchSizeX * c, 0, 0 };
				for (int i = 1; i < 4; i++)
				{
					if (r == 1)
						tempPoints.push_back(points[16 + (c - 1) * 12 + (i - 1) * 4 + 3]);
					else
						tempPoints.push_back(points[12 + (c - 1) * 9 + 16 + (sizeX - 1) * 12 + (12 + 9 * (sizeX - 1)) * (r - 2) + (i - 1) * 3 + 2]); // even more magic
					for (int j = 1; j < 4; j++)
					{
						float angle = minAngleDist * (4 + 3 * (r - 1) + j - 1);
						points.emplace_back(new Point(offset + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + +radius * glm::vec3{ 0, std::cos(angle), std::sin(angle) }));
						tempPoints.push_back(points[points.size() - 1]);
					}
				}
				patches.emplace_back(new Patch(tempPoints));
			}
			tempPoints.clear();
		}
		
		// first patch in the last row
		for (int i = 0; i < 4; i++)
		{
			int r = sizeZ - 1;
			tempPoints.push_back(points[16 + (sizeX - 1) * 12 + (12 + 9 * (sizeX - 1)) * (r - 2) + i * 3 + 2]);
			for (int j = 1; j < 3; j++)
			{
				float angle = minAngleDist * (4 + 3 * (r - 1) + j - 1);
				points.emplace_back(new Point(position + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + radius * glm::vec3{ 0, std::cos(angle), std::sin(angle) }));
				tempPoints.push_back(points[points.size() - 1]);
			}
			tempPoints.push_back(points[i * 4]);
		}
		patches.emplace_back(new Patch(tempPoints));

		// the rest of the last row
		for (int c = 1; c < sizeX; c++)
		{
			tempPoints.erase(tempPoints.begin(), tempPoints.begin() + 12);
			glm::vec3 offset = position + glm::vec3{ patchSizeX * c, 0, 0 };
			for (int i = 1; i < 4; i++)
			{
				int r = sizeZ - 1;
				tempPoints.push_back(points[12 + (c - 1) * 9 + 16 + (sizeX - 1) * 12 + (12 + 9 * (sizeX - 1)) * (r - 2) + (i - 1) * 3 + 2]); // even more magic
				for (int j = 1; j < 3; j++)
				{
					float angle = minAngleDist * (4 + 3 * (r - 1) + j - 1);
					points.emplace_back(new Point(offset + glm::vec3{ patchSizeX * i / 3.0f, 0, 0 } + +radius * glm::vec3{ 0, std::cos(angle), std::sin(angle) }));
					tempPoints.push_back(points[points.size() - 1]);
				}
				tempPoints.push_back(points[16 + (c - 1) * 12 + (i - 1) * 4]);
			}
			patches.emplace_back(new Patch(tempPoints));
		}
		tempPoints.clear();

		attachPoints();
	}

	C0Surface(const MG1::BezierSurfaceC0& other, const std::vector<std::unique_ptr<Point>>& allPoints) : Surface(other.name, other.size.x, other.size.y, other.vWrapped)
	{
		instanceCount++;
		if (!other.name.empty())
			setName(other.name);

		int sizeX = other.size.x;
		int sizeZ = other.size.y;

		std::vector<Point*> tempPoints;

		for (auto& patch : other.patches)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					auto point = findPoint(allPoints, patch.controlPoints[i * 4 + j].GetId());
					tempPoints.push_back(point);
					points.push_back(point);
				}
			}
			patches.emplace_back(new Patch(tempPoints));
			tempPoints.clear();
		}

		attachPoints();
	}

	virtual void drawPolygon(const Shader* shader) const override
	{
		for (auto&& patch : patches)
		{
			patch->drawPolygon(shader);
		}
	}

	virtual void draw(const Shader* shader) const override
	{
		setColor(shader);
		Parametric::uploadTrimTexture(shader);
		
		for (int i = 0; i < patches.size(); i++)
		{
			int z = i / sizeX;
			int x = i % sizeX;
			shader->setFloat("patchIndexU", z);
			shader->setFloat("patchIndexV", x);
			shader->setFloat("patchCountU", sizeZ);
			shader->setFloat("patchCountV", sizeX);

			shader->setInt("reverse", false);
			shader->setFloat("segmentCount", densityZ);
			patches[i]->draw(shader);

			shader->setInt("reverse", true);
			shader->setFloat("segmentCount", densityX);
			patches[i]->draw(shader);
		}
	}

	virtual void update() override
	{
		if (!scheduledToUpdate)
			return;

		for (auto&& patch : patches)
		{
			patch->update();
		}

		scheduledToUpdate = false;
	}

	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<std::unique_ptr<Point>>& allPoints) const override
	{
		MG1::BezierSurfaceC0 surface;
		surface.name = getName();
		surface.size.x = sizeX;
		surface.size.y = sizeZ;

		surface.uWrapped = false;
		surface.vWrapped = cylinder;

		for (auto&& patch : patches)
		{
			patch->addToMGSurface(surface, allPoints);
		}
		if (cylinder)
			transposeSurface(surface);
		mgscene.surfacesC0.push_back(surface);
	}

	virtual Shader* getPreferredShader() const
	{
		return preferredShader;
	}

	inline static void setPreferredShader(Shader* shader)
	{
		preferredShader = shader;
	}

	virtual void collapsePoints(const Point* oldPoint1, const Point* oldPoint2, Point* newPoint) override
	{
		Shape::collapsePoints(oldPoint1, oldPoint2, newPoint);
		for (auto&& patch : patches)
		{
			patch->replacePoint(oldPoint1, newPoint);
		}
	}

	Border getBorder() const
	{
		if (sizeX == 0 || sizeZ == 0)
			return {};

		if (cylinder)
		{
			return
			Border{
				BorderNode{patches[0]->getPoints()[0], patches[0]->getPoints()[4], nullptr},
				BorderNode{patches[0]->getPoints()[1], patches[0]->getPoints()[5], nullptr},
				BorderNode{patches[0]->getPoints()[2], patches[0]->getPoints()[6], nullptr},
				BorderNode{patches[0]->getPoints()[3], patches[0]->getPoints()[7], nullptr},
				BorderNode{patches[1]->getPoints()[1], patches[1]->getPoints()[5], nullptr},
				BorderNode{patches[1]->getPoints()[2], patches[1]->getPoints()[6], nullptr},
				BorderNode{patches[1]->getPoints()[3], patches[1]->getPoints()[7], nullptr},
				BorderNode{patches[2]->getPoints()[1], patches[2]->getPoints()[5], nullptr},
				BorderNode{patches[2]->getPoints()[2], patches[2]->getPoints()[6], nullptr},

				BorderNode{patches[sizeX * sizeZ - 3]->getPoints()[12], patches[sizeX * sizeZ - 3]->getPoints()[8], nullptr},
				BorderNode{patches[sizeX * sizeZ - 3]->getPoints()[13], patches[sizeX * sizeZ - 3]->getPoints()[9], nullptr},
				BorderNode{patches[sizeX * sizeZ - 3]->getPoints()[14], patches[sizeX * sizeZ - 3]->getPoints()[10], nullptr},
				BorderNode{patches[sizeX * sizeZ - 3]->getPoints()[15], patches[sizeX * sizeZ - 3]->getPoints()[11], nullptr},
				BorderNode{patches[sizeX * sizeZ - 2]->getPoints()[13], patches[sizeX * sizeZ - 2]->getPoints()[9], nullptr},
				BorderNode{patches[sizeX * sizeZ - 2]->getPoints()[14], patches[sizeX * sizeZ - 2]->getPoints()[10], nullptr},
				BorderNode{patches[sizeX * sizeZ - 2]->getPoints()[15], patches[sizeX * sizeZ - 2]->getPoints()[11], nullptr},
				BorderNode{patches[sizeX * sizeZ - 1]->getPoints()[13], patches[sizeX * sizeZ - 1]->getPoints()[9], nullptr},
				BorderNode{patches[sizeX * sizeZ - 1]->getPoints()[14], patches[sizeX * sizeZ - 1]->getPoints()[10], nullptr}
			};
		}

		Border border;
		for (int i = 0; i < sizeZ; i++)
		{
			auto& tempPoints = patches[i]->getPoints();
			for (int j = 0; j < 3; j++)
			{		
				if (i == 0 && j == 0) [[unlikely]]
					border.emplace_back(tempPoints[j], tempPoints[4], tempPoints[1]);
				else [[likely]] if (i == sizeZ - 1 && j == 3) [[unlikely]]
					border.emplace_back(tempPoints[j], tempPoints[2], tempPoints[7]);
				else [[likely]]
					border.emplace_back(tempPoints[j], tempPoints[j + 4], tempPoints[j + 4]);
			}
		}
		border.emplace_back(patches[sizeZ - 1]->getPoints()[3], patches[sizeZ - 1]->getPoints()[2], patches[sizeZ - 1]->getPoints()[7]);

		if (sizeX == 1)
		{
			auto& tempPoints0 = patches[0]->getPoints();
			border.emplace_back(tempPoints0[7], tempPoints0[6], tempPoints0[6]);
			border.emplace_back(tempPoints0[11], tempPoints0[10], tempPoints0[10]);
		}
		else
		{
			auto& tempPoints0 = patches[sizeZ - 1]->getPoints();
			border.emplace_back(tempPoints0[7], tempPoints0[6], tempPoints0[6]);
			border.emplace_back(tempPoints0[11], tempPoints0[10], tempPoints0[10]);

			for (int i = 1; i < sizeX; i++)
			{
				auto& tempPoints = patches[(i + 1) * sizeZ - 1]->getPoints();
				for (int j = 0; j < 3; j++)
				{
					border.emplace_back(tempPoints[4 * j + 3], tempPoints[4 * j + 2], tempPoints[4 * j + 2]);
				}
			}
		}	

		for (int i = sizeZ - 1; i >= 0; i--)
		{
			auto& tempPoints = patches[(sizeX - 1) * sizeZ + i]->getPoints();
			for (int j = 0; j < 3; j++)
			{
				if (i == sizeZ - 1 && j == 0) [[unlikely]]
					border.emplace_back(tempPoints[15 - j], tempPoints[11], tempPoints[14]);					
				else [[likely]]
					border.emplace_back(tempPoints[15 - j], tempPoints[11 - j], tempPoints[11 - j]);
			}		
		}
		border.emplace_back(patches[(sizeX - 1) * sizeZ]->getPoints()[12], patches[(sizeX - 1) * sizeZ]->getPoints()[13], patches[(sizeX - 1) * sizeZ]->getPoints()[8]);

		if (sizeX == 1)
		{
			auto& tempPoints2 = patches[0]->getPoints();
			border.emplace_back(tempPoints2[8], tempPoints2[9], tempPoints2[9]);
			border.emplace_back(tempPoints2[4], tempPoints2[5], tempPoints2[5]);
		}
		else
		{
			auto& tempPoints2 = patches[(sizeX - 1) * sizeZ]->getPoints();
			border.emplace_back(tempPoints2[8], tempPoints2[9], tempPoints2[9]);
			border.emplace_back(tempPoints2[4], tempPoints2[5], tempPoints2[5]);

			for (int i = sizeX - 2; i >= 1; i--)
			{
				auto& tempPoints = patches[i * sizeZ]->getPoints();
				for (int j = 0; j < 3; j++)
				{
					border.emplace_back(tempPoints[12 - 4 * j], tempPoints[13 - 4 * j], tempPoints[13 - 4 * j]);
				}
			}
			auto& tempPoints3 = patches[0]->getPoints();
			border.emplace_back(tempPoints3[12], tempPoints3[13], tempPoints3[13]);
			border.emplace_back(tempPoints3[8], tempPoints3[9], tempPoints3[9]);
		}		

		return border;
	}

	virtual glm::vec3 evaluate(float u, float v) const override
	{
		return eval(u, v, [](float u, float v, const std::unique_ptr<Patch>& patch) { return patch->evaluate(u, v); });
	}

	virtual glm::vec3 derivativeU(float u, float v) const override
	{
		if (u > 1.0f - math::derivativeH)
			return derivativeU(u - math::derivativeH, v);
		return (evaluate(u + math::derivativeH, v) - evaluate(u, v)) / math::derivativeH;
		//return eval(u, v, [](float u, float v, const std::unique_ptr<Patch>& patch) { return patch->derivativeU(u, v); });

	}

	virtual glm::vec3 derivativeV(float u, float v) const override
	{
		if (v > 1.0f - math::derivativeH)
			return derivativeV(u, v - math::derivativeH);
		return (evaluate(u, v + math::derivativeH) - evaluate(u, v)) / math::derivativeH;
		///return eval(u, v, [](float u, float v, const std::unique_ptr<Patch>& patch) { return patch->derivativeV(u, v); });
	}

private:
	inline static unsigned int instanceCount = 0;
	inline static Shader* preferredShader = nullptr;

	std::vector<std::unique_ptr<Patch>> patches;

	virtual std::string getSurfaceName() const override
	{
		return std::format("{} {}", "C0 Surface", instanceCount++);
	}

	glm::vec3 eval(float u, float v, auto func) const
	{
		// sizeZ <=> sizeX ?
		float fIndexV = v * sizeX;
		int indexV = fIndexV;
		float newV = fIndexV - indexV;
		float fIndexU = u * sizeZ;
		int indexU = fIndexU;
		float newU = fIndexU - indexU;

		if (v >= 1.0f - math::eps)
		{
			indexV = sizeX - 1;
			newV = 1.0f;
		}
		if (u >= 1.0f - math::eps)
		{
			indexU = sizeZ - 1;
			newU = 1.0f;
		}

		/*if (cylinder && newU >= 1.0f - math::eps)
		{
			int nextU = (indexU + 1) % sizeZ;
			return (func(1.0f, newV, patches[indexU * sizeX + indexV]) + func(0.0f, newV, patches[nextU * sizeX + indexV])) * 0.5f;
		}

		if (cylinder && newU <= math::eps)
		{
			int prevU = indexU == 0 ? (sizeZ - 1) : indexU - 1;
			return (func(0.0f, newV, patches[indexU * sizeX + indexV]) + func(1.0f, newV, patches[prevU * sizeX + indexV])) * 0.5f;
		}*/

		return func(newU, newV, patches[indexU * sizeX + indexV]);
	}
};