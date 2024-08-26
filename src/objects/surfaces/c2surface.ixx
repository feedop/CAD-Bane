export module c2surface;

import <glad/glad.h>;
import <format>;
import <span>;
import <vector>;
import <glm/vec3.hpp>;

import colors;
import glutils;
import math;
import surface;
import mg1utils;

export class C2Surface : public Surface
{
public:
	C2Surface(const glm::vec3& position, int sizeX, int sizeZ) : Surface(getSurfaceName(), sizeX, sizeZ)
	{
		genBuffers();

		for (int i = 0; i < sizeX; i++)
		{
			auto offset = i * distanceX;
			for (int j = 0; j < sizeZ; j++)
			{
				points.emplace_back(new Point(position + glm::vec3{ offset, 0, j * distanceZ }));
			}
		}
		attachPoints();
		update();
	}

	C2Surface(const glm::vec3& position, int sizeX, float radius) : Surface(getSurfaceName(), sizeX, std::max(3, static_cast<int>(2 * math::pi * radius / distanceZ)), true)
	{
		genBuffers();

		float minAngleDist = 2 * math::pi / sizeZ;
		for (int i = 0; i < sizeX; i++)
		{
			auto offset = i * distanceX;
			for (int j = 0; j < sizeZ; j++)
			{
				float angle = minAngleDist * j;
				points.emplace_back(new Point(position + glm::vec3{ offset, 0, 0 } + radius * glm::vec3{ 0, std::cos(angle), std::sin(angle) }));
			}
		}

		attachPoints();
		update();
	}


	C2Surface(const MG1::BezierSurfaceC2& other, const std::vector<std::unique_ptr<Point>>& allPoints) :
		Surface(other.name, other.size.x + 3, other.size.y, other.vWrapped)
	{
		instanceCount++;
		if (!other.name.empty())
			setName(other.name);

		if (!cylinder)
			sizeZ += 3;

		genBuffers();

		points.resize(sizeX * sizeZ);
		
		for (auto i = 0; i < other.size.y; i++)
		{
			for (auto j = 0; j < other.size.x - 1; j++)
			{
				// first element only - the rest will be added by next patches
				auto& patch = other.patches[i * other.size.x + j];
				auto index = j * sizeZ + i;
				points[index] = findPoint(allPoints, patch.controlPoints[0].GetId());
			}
			// last patch - add 4 elements
			{
				auto j = other.size.x - 1;
				auto& patch = other.patches[i * other.size.x + j];
				for (int k = 0; k < 4; k++)
					points[(j + k) * sizeZ + i] = findPoint(allPoints, patch.controlPoints[k].GetId());
			}
		}
		if (!cylinder) // extra 3 rows
		{
			auto i = other.size.y - 1;
			for (auto j = 0; j < other.size.x; j++)
			{		
				auto& patch = other.patches[i * other.size.x + j];
				for (int kk = 1; kk < 4; kk++)
					points[j * sizeZ + i + kk] = findPoint(allPoints, patch.controlPoints[4 * kk].GetId());

			}
			// last patch - add 4 elements
			{
				auto j = other.size.x - 1;
				auto& patch = other.patches[i * other.size.x + j];
				for (int k = 1; k < 4; k++)
				{
					for (int kk = 1; kk < 4; kk++)
					{
						points[(j + k) * sizeZ + i + kk] = findPoint(allPoints, patch.controlPoints[4*kk + k].GetId());
					}
				}			
			}
		}
		
		attachPoints();
		update();
	}

	virtual void drawPolygon(const Shader* shader) const override
	{
		shader->setVector("color", polygonColor);
		shader->setMatrix("model", glm::mat4{ 1.0f });
		{
			ScopedBindArray ba(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygonEBO);
			// Vertical
			glDrawElements(GL_LINES, polygonIndexCount, GL_UNSIGNED_INT, 0);
		}
	}

	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		setColor(shader);

		shader->setInt("knotCountX", sizeX + 3);
		shader->setInt("knotCountZ", sizeZ + 3);

		auto indexSizeZ = cylinder ? sizeZ : sizeZ - 3;
		for (int i = 0; i < sizeX - 3; i++)
		{
			for (int j = 0; j < indexSizeZ; j++)
			{
				shader->setInt("knotIndexX", 3 + j);
				shader->setInt("knotIndexZ", 3 + i);
				
				shader->setInt("reverse", false);
				shader->setFloat("segmentCount", densityZ);
				glDrawElements(GL_PATCHES, 16, GL_UNSIGNED_INT, (void*)(16 * sizeof(unsigned int) * (i * indexSizeZ + j)));

				shader->setInt("reverse", true);
				shader->setFloat("segmentCount", densityX);
				glDrawElements(GL_PATCHES, 16, GL_UNSIGNED_INT, (void*)(16 * sizeof(unsigned int) * (i * indexSizeZ + j)));
			}		
		}
	}

	virtual void update() override
	{
		if (!scheduledToUpdate)
			return;

		positions.resize(points.size());
		for (int i = 0; i < points.size(); i++)
		{
			positions[i] = points[i]->getPosition();
		}

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		scheduledToUpdate = false;
	}

	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<std::unique_ptr<Point>>& allPoints) const override
	{
		auto find = [&](const Point* point)
		{
			for (int i = 0; i < allPoints.size(); i++)
			{
				if (allPoints[i].get() == point)
					return i;
			}
			return 0;
		};

		MG1::BezierSurfaceC2 surface;
		surface.name = getName();
		surface.size.x = sizeX - 3;
		surface.size.y = cylinder ? sizeZ : sizeZ - 3;
		surface.uWrapped = false;
		surface.vWrapped = cylinder;

		for (int i = 0; i < sizeZ - 3; i++)
		{
			for (int j = 0; j < sizeX - 3; j++)
			{
				MG1::BezierPatchC2 bp;
				bp.samples.x = 4;
				bp.samples.y = 4;
				for (int r = 0; r < 4; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						auto point = points[(j + c) * sizeZ + i + r];
						bp.controlPoints.emplace_back(find(point) + 1);
					}
				}
				surface.patches.push_back(bp);
			}
		}
		if (cylinder)
		{
			// 3 last rows
			auto i = sizeZ - 3;
			for (int j = 0; j < sizeX - 3; j++)
			{
				MG1::BezierPatchC2 bp;
				bp.samples.x = 4;
				bp.samples.y = 4;
				for (int r = 0; r < 3; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						auto point = points[(j + c) * sizeZ + i + r];
						bp.controlPoints.emplace_back(find(point) + 1);
					}
				}
				for (int r = 0; r < 1; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						auto point = points[(j + c) * sizeZ + i + r];
						bp.controlPoints.emplace_back(find(point) + 1);
					}
				}
				surface.patches.push_back(bp);
			}

			i = sizeZ - 2;
			for (int j = 0; j < sizeX - 3; j++)
			{
				MG1::BezierPatchC2 bp;
				bp.samples.x = 4;
				bp.samples.y = 4;
				for (int r = 0; r < 2; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						auto point = points[(j + c) * sizeZ + i + r];
						bp.controlPoints.emplace_back(find(point) + 1);
					}
				}
				for (int r = 0; r < 2; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						auto point = points[(j + c) * sizeZ + r];
						bp.controlPoints.emplace_back(find(point) + 1);
					}
				}
				surface.patches.push_back(bp);
			}

			i = sizeZ - 1;
			for (int j = 0; j < sizeX - 3; j++)
			{
				MG1::BezierPatchC2 bp;
				bp.samples.x = 4;
				bp.samples.y = 4;
				for (int r = 0; r < 1; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						auto point = points[(j + c) * sizeZ + i + r];
						bp.controlPoints.emplace_back(find(point) + 1);
					}
				}
				for (int r = 0; r < 3; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						auto point = points[(j + c) * sizeZ + r];
						bp.controlPoints.emplace_back(find(point) + 1);
					}
				}
				surface.patches.push_back(bp);
			}
		}
		if (cylinder)
			transposeSurface(surface);
		mgscene.surfacesC2.push_back(surface);
	}

	virtual Shader* getPreferredShader() const
	{
		return preferredShader;
	}

	inline static void setPreferredShader(Shader* shader)
	{
		preferredShader = shader;
	}

private:
	inline static unsigned int instanceCount = 0;
	inline static Shader* preferredShader = nullptr;
	inline static const glm::vec4 polygonColor = colors::orange;

	static constexpr float distanceX = patchSizeX / 3;
	static constexpr float distanceZ = patchSizeZ / 3;

	unsigned int polygonEBO = 0;
	unsigned int polygonIndexCount = 0;

	std::vector<glm::vec3> positions;

	virtual std::string getSurfaceName() const override
	{
		return std::format("{} {}", "C2 Surface", instanceCount++);
	}

	void genBuffers()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &polygonEBO);

		// fill indices
		std::vector<unsigned int> indices;
		for (int i = 0; i < sizeX - 3; i++)
		{
			for (int j = 0; j < sizeZ - 3; j++)
			{
				for (int c = 0; c < 4; c++)
				{
					unsigned int index = (i + c) * sizeZ + j;
					indices.push_back(index);
					indices.push_back(index + 1);
					indices.push_back(index + 2);
					indices.push_back(index + 3);
				}
			}
			if (cylinder)
			{
				for (int c = 0; c < 4; c++)
				{
					unsigned int index = (i + c) * sizeZ;
					indices.push_back(index + sizeZ - 3);
					indices.push_back(index + sizeZ - 2);
					indices.push_back(index + sizeZ - 1);
					indices.push_back(index);
				}
				for (int c = 0; c < 4; c++)
				{
					unsigned int index = (i + c) * sizeZ;
					indices.push_back(index + sizeZ - 2);
					indices.push_back(index + sizeZ - 1);
					indices.push_back(index);
					indices.push_back(index + 1);
				}
				for (int c = 0; c < 4; c++)
				{
					unsigned int index = (i + c) * sizeZ;
					indices.push_back(index + sizeZ - 1);
					indices.push_back(index);
					indices.push_back(index + 1);
					indices.push_back(index + 2);
				}
			}
		}

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

		std::vector<unsigned int> polygonIndices;
		for (int i = 0; i < sizeX; i++)
		{
			for (int j = 0; j < sizeZ - 1; j++)
			{
				polygonIndices.push_back(i * sizeZ + j);
				polygonIndices.push_back(i * sizeZ + j + 1);
			}
			if (cylinder)
			{
				polygonIndices.push_back(i * sizeZ + sizeZ - 1);
				polygonIndices.push_back(i * sizeZ);
			}
		}
		for (int j = 0; j < sizeZ; j++)
		{
			for (int i = 0; i < sizeX - 1; i++)
			{
				polygonIndices.push_back(i * sizeZ + j);
				polygonIndices.push_back((i + 1) * sizeZ + j);
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygonEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * polygonIndices.size(), &polygonIndices[0], GL_STATIC_DRAW);
		polygonIndexCount = polygonIndices.size();
	}
};