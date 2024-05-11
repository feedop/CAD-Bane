export module c0surface;

import <format>;
import <span>;
import <glm/vec3.hpp>;

import math;
import surface;

export class C0Surface : public Surface
{
public:
	C0Surface(const glm::vec3& position, int sizeX, int sizeZ) : Surface(getSurfaceName())
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

		for (auto&& point : points)
		{
			point->attach(this);
		}
	}

	C0Surface(const glm::vec3& position, int sizeX, float radius) : Surface(getSurfaceName())
	{
		float realLength = 2 * math::pi * radius;
		int sizeZ = std::max(4, static_cast<int>(realLength / patchSizeZ));
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

		for (auto&& point : points)
		{
			point->attach(this);
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

private:
	inline static unsigned int instanceCount = 0;

	virtual std::string getSurfaceName() const override
	{
		return std::format("{} {}", "C0 Surface", instanceCount++);
	}
};