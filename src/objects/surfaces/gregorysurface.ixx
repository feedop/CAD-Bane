export module gregorysurface;

import std;
import glm;

import <glad/glad.h>;

import colors;
import glutils;
import gregorystructs;
import math;
import patch;
import point;
import surface;

using namespace math;
using namespace Gregory;

/// <summary>
/// Class representing a Gregory Surface, a specific type of surface defined by a hole.
/// </summary>
export class GregorySurface : public Surface
{
public:
	/// <summary>
	/// Constructor to create a Gregory surface from a specified hole.
	/// </summary>
	/// <param name="hole">The hole that defines the surface geometry.</param>
	GregorySurface(const Hole& hole) : Surface(getSurfaceName(), 1, 1), hole(hole)
	{
		genBuffers();
		fillPoints();
		attachPoints();
		positions.resize(60);
		vectorPositions.resize(48);

		update();
	}

	/// <summary>
	/// Destructor to clean up resources used by the Gregory surface.
	/// </summary>
	virtual ~GregorySurface()
	{
		ScopedBindArray ba(vectorVAO);

		glBindBuffer(GL_ARRAY_BUFFER, vectorVBO);
		glDeleteBuffers(1, &vectorVBO);

		glDeleteVertexArrays(1, &vectorVAO);
	}

	/// <summary>
	/// Draw the surface as polygons using the specified shader.
	/// </summary>
	/// <param name="shader">The shader to be used for rendering.</param>
	virtual void drawPolygon(const Shader* shader) const override
	{
		ScopedBindArray ba(vectorVAO);
		shader->setVector("color", polygonColor);
		shader->setMatrix("model", glm::mat4{ 1.0f });
		{
			glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vectorPositions.size()));
		}
	}

	/// <summary>
	/// Draw the surface using the specified shader.
	/// </summary>
	/// <param name="shader">The shader to be used for rendering.</param>
	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);

		setColor(shader);
		shader->setInt("reverse", false);
		shader->setFloat("segmentCount", densityZ);
		glPatchParameteri(GL_PATCH_VERTICES, 20);
		glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(positions.size()));

		shader->setInt("reverse", true);
		shader->setFloat("segmentCount", densityX);
		glPatchParameteri(GL_PATCH_VERTICES, 20);
		glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(positions.size()));
	}

	// <summary>
	/// Update the surface's geometry if it is scheduled for an update.
	/// </summary>
	virtual void update() override
	{
		if (!scheduledToUpdate)
			return;

		calculatePositions();
		uploadPositions();

		scheduledToUpdate = false;
	}

	/// <summary>
	/// Not serializable
	/// </summary>
	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<std::unique_ptr<Point>>& allPoints) const override
	{
		
	}

	/// <summary>
	/// Get the preferred shader for rendering this surface.
	/// </summary>
	/// <returns>The preferred shader.</returns>
	virtual Shader* getPreferredShader() const
	{
		return preferredShader;
	}

	/// <summary>
	/// Set the preferred shader to be used for rendering GregorySurface objects.
	/// </summary>
	/// <param name="shader">The shader to be set as the preferred shader.</param>
	inline static void setPreferredShader(Shader* shader)
	{
		preferredShader = shader;
	}

private:
	inline static unsigned int instanceCount = 0;
	inline static Shader* preferredShader = nullptr;
	inline static const glm::vec4 polygonColor = colors::cyan;

	unsigned int vectorVAO = 0, vectorVBO = 0;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> vectorPositions;
	const Hole hole;

	virtual std::string getSurfaceName() const override
	{
		return std::format("{} {}", "Gregory Patch", instanceCount++);
	}

	/// <summary>
	/// Generate OpenGL buffers for the surface's geometry and vectors.
	/// </summary>
	void genBuffers()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		glGenVertexArrays(1, &vectorVAO);
		glGenBuffers(1, &vectorVBO);
	}

	/// <summary>
	/// Fill the points for the Gregory surface based on the hole.
	/// </summary>
	void fillPoints()
	{
		for (int i = 0; i < 4; i++)
		{
			points.push_back(hole[0][i].p);
		}
		for (int i = 1; i < 4; i++)
		{
			points.push_back(hole[1][i].p);
		}
		points.push_back(hole[2][1].p);
		points.push_back(hole[2][2].p);

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				points.push_back(hole[i][j].prev);
			}
		}
	}

	/// <summary>
	/// Upload the calculated positions to OpenGL buffers.
	/// </summary>
	void uploadPositions()
	{
		{
			ScopedBindArray ba(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}	
		{
			ScopedBindArray ba(vectorVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vectorVBO);
			glBufferData(GL_ARRAY_BUFFER, vectorPositions.size() * sizeof(glm::vec3), &vectorPositions[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}
	}

	/// <summary>
	/// Calculate the positions of the surface's vertices.
	/// </summary>
	void calculatePositions()
	{
		auto side0 = deCasteljauSplit3(
			hole[0][0].p->getPosition(),
			hole[0][1].p->getPosition(),
			hole[0][2].p->getPosition(),
			hole[0][3].p->getPosition(),
			0.5f);
		auto side1 = deCasteljauSplit3(
			hole[1][0].p->getPosition(),
			hole[1][1].p->getPosition(),
			hole[1][2].p->getPosition(),
			hole[1][3].p->getPosition(),
			0.5f);
		auto side2 = deCasteljauSplit3(
			hole[2][0].p->getPosition(),
			hole[2][1].p->getPosition(),
			hole[2][2].p->getPosition(),
			hole[2][3].p->getPosition(),
			0.5f);

		auto side0prev = deCasteljauSplit3(
			hole[0][0].prev->getPosition(),
			hole[0][1].prev->getPosition(),
			hole[0][2].prev->getPosition(),
			hole[0][3].prev->getPosition(),
			0.5f);
		auto side1prev = deCasteljauSplit3(
			hole[1][0].prev->getPosition(),
			hole[1][1].prev->getPosition(),
			hole[1][2].prev->getPosition(),
			hole[1][3].prev->getPosition(),
			0.5f);
		auto side2prev = deCasteljauSplit3(
			hole[2][0].prev->getPosition(),
			hole[2][1].prev->getPosition(),
			hole[2][2].prev->getPosition(),
			hole[2][3].prev->getPosition(),
			0.5f);

		auto& P30 = side0[3];
		auto& P31 = side1[3];
		auto& P32 = side2[3];

		auto& Prev30 = side0prev[3];
		auto& Prev31 = side1prev[3];
		auto& Prev32 = side2prev[3];

		auto P20 = P30 + P30 - Prev30;
		auto P21 = P31 + P31 - Prev31;
		auto P22 = P32 + P32 - Prev32;

		auto Q0 = (P20 * 3.0f - P30) * 0.5f;
		auto Q1 = (P21 * 3.0f - P31) * 0.5f;
		auto Q2 = (P22 * 3.0f - P32) * 0.5f;
		auto P = (Q0 + Q1 + Q2) * (1.0f/3);

		auto P10 = (Q0 * 2.0f + P) * (1.0f / 3);
		auto P11 = (Q1 * 2.0f + P) * (1.0f / 3);
		auto P12 = (Q2 * 2.0f + P) * (1.0f / 3);
	
		{
			// Patch 0
			auto a = P - P12;
			auto b = P11 - P;
			auto g2 = (a + b) * 0.5f;
			auto g0 = side0[4] - side0[3];
			auto g1 = (g0 + g2) * 0.5f;
			auto f7 = P20 + deCasteljau2(g0, g1, g2, 1.0f / 3);
			auto f6 = P10 + deCasteljau2(g0, g1, g2, 2.0f / 3);

			b = P10 - P;
			g2 = (a + b) * 0.5f;
			g0 = side1[2] - side1[3];
			g1 = (g0 + g2) * 0.5f;
			auto f5 = P11 + deCasteljau2(g0, g1, g2, 2.0f / 3);
			auto f4 = P21 + deCasteljau2(g0, g1, g2, 1.0f / 3);

			auto f0 = side0[4] + side0[4] - side0prev[4];
			auto f1 = side0[5] + side0[5] - side0prev[5];
			auto f2 = side1[1] + side1[1] - side1prev[1];
			auto f3 = side1[2] + side1[2] - side1prev[2];

			positions[0] = side0[3];
			positions[1] = side0[6];
			positions[2] = side1[3];
			positions[3] = P;

			positions[4] = side0[4];
			positions[5] = side0[5];
			positions[6] = side1[1];
			positions[7] = side1[2];
			positions[8] = P21;
			positions[9] = P11;
			positions[10] = P10;
			positions[11] = P20;

			positions[12] = f0;
			positions[13] = f1;
			positions[14] = f2;
			positions[15] = f3;
			positions[16] = f4;
			positions[17] = f5;
			positions[18] = f6;
			positions[19] = f7;

			vectorPositions[0] = side0[4];
			vectorPositions[1] = f0;
			vectorPositions[2] = side0[5];
			vectorPositions[3] = f1;
			vectorPositions[4] = side1[1];
			vectorPositions[5] = f2;
			vectorPositions[6] = side1[2];
			vectorPositions[7] = f3;

			vectorPositions[8] = P21;
			vectorPositions[9] = f4;
			vectorPositions[10] = P11;
			vectorPositions[11] = f5;
			vectorPositions[12] = P10;
			vectorPositions[13] = f6;
			vectorPositions[14] = P20;
			vectorPositions[15] = f7;
		}
		{
			// Patch 1
			static constexpr auto offset = 20;
			static constexpr auto vecOffset = 16;
			auto a = P - P10;
			auto b = P12 - P;
			auto g2 = (a + b) * 0.5f;
			auto g0 = side1[4] - side1[3];
			auto g1 = (g0 + g2) * 0.5f;
			auto f7 = P21 + deCasteljau2(g0, g1, g2, 1.0f / 3);
			auto f6 = P11 + deCasteljau2(g0, g1, g2, 2.0f / 3);

			b = P11 - P;
			g2 = (a + b) * 0.5f;
			g0 = side2[2] - side2[3];
			g1 = (g0 + g2) * 0.5f;
			auto f5 = P12 + deCasteljau2(g0, g1, g2, 2.0f / 3);
			auto f4 = P22 + deCasteljau2(g0, g1, g2, 1.0f / 3);

			auto f0 = side1[4] + side1[4] - side1prev[4];
			auto f1 = side1[5] + side1[5] - side1prev[5];
			auto f2 = side2[1] + side2[1] - side2prev[1];
			auto f3 = side2[2] + side2[2] - side2prev[2];

			positions[offset + 0] = side1[3];
			positions[offset + 1] = side1[6];
			positions[offset + 2] = side2[3];
			positions[offset + 3] = P;

			positions[offset + 4] = side1[4];
			positions[offset + 5] = side1[5];
			positions[offset + 6] = side2[1];
			positions[offset + 7] = side2[2];
			positions[offset + 8] = P22;
			positions[offset + 9] = P12;
			positions[offset + 10] = P11;
			positions[offset + 11] = P21;

			positions[offset + 12] = f0;
			positions[offset + 13] = f1;
			positions[offset + 14] = f2;
			positions[offset + 15] = f3;
			positions[offset + 16] = f4;
			positions[offset + 17] = f5;
			positions[offset + 18] = f6;
			positions[offset + 19] = f7;

			vectorPositions[vecOffset + 0] = side1[4];
			vectorPositions[vecOffset + 1] = f0;
			vectorPositions[vecOffset + 2] = side1[5];
			vectorPositions[vecOffset + 3] = f1;
			vectorPositions[vecOffset + 4] = side2[1];
			vectorPositions[vecOffset + 5] = f2;
			vectorPositions[vecOffset + 6] = side2[2];
			vectorPositions[vecOffset + 7] = f3;

			vectorPositions[vecOffset + 8] = P22;
			vectorPositions[vecOffset + 9] = f4;
			vectorPositions[vecOffset + 10] = P12;
			vectorPositions[vecOffset + 11] = f5;
			vectorPositions[vecOffset + 12] = P11;
			vectorPositions[vecOffset + 13] = f6;
			vectorPositions[vecOffset + 14] = P21;
			vectorPositions[vecOffset + 15] = f7;
		}
		{
			// Patch 2
			static constexpr auto offset = 40;
			static constexpr auto vecOffset = 32;
			auto a = P - P11;
			auto b = P10 - P;
			auto g2 = (a + b) * 0.5f;
			auto g0 = side2[4] - side2[3];
			auto g1 = (g0 + g2) * 0.5f;
			auto f7 = P22 + deCasteljau2(g0, g1, g2, 1.0f / 3);
			auto f6 = P12 + deCasteljau2(g0, g1, g2, 2.0f / 3);

			b = P12 - P;
			g2 = (a + b) * 0.5f;
			g0 = side0[2] - side0[3];
			g1 = (g0 + g2) * 0.5f;
			auto f5 = P10 + deCasteljau2(g0, g1, g2, 2.0f / 3);
			auto f4 = P20 + deCasteljau2(g0, g1, g2, 1.0f / 3);

			auto f0 = side2[4] + side2[4] - side2prev[4];
			auto f1 = side2[5] + side2[5] - side2prev[5];
			auto f2 = side0[1] + side0[1] - side0prev[1];
			auto f3 = side0[2] + side0[2] - side0prev[2];

			positions[offset + 0] = side2[3];
			positions[offset + 1] = side2[6];
			positions[offset + 2] = side0[3];
			positions[offset + 3] = P;

			positions[offset + 4] = side2[4];
			positions[offset + 5] = side2[5];
			positions[offset + 6] = side0[1];
			positions[offset + 7] = side0[2];
			positions[offset + 8] = P20;
			positions[offset + 9] = P10;
			positions[offset + 10] = P12;
			positions[offset + 11] = P22;

			positions[offset + 12] = f0;
			positions[offset + 13] = f1;
			positions[offset + 14] = f2;
			positions[offset + 15] = f3;
			positions[offset + 16] = f4;
			positions[offset + 17] = f5;
			positions[offset + 18] = f6;
			positions[offset + 19] = f7;

			vectorPositions[vecOffset + 0] = side2[4];
			vectorPositions[vecOffset + 1] = f0;
			vectorPositions[vecOffset + 2] = side2[5];
			vectorPositions[vecOffset + 3] = f1;
			vectorPositions[vecOffset + 4] = side0[1];
			vectorPositions[vecOffset + 5] = f2;
			vectorPositions[vecOffset + 6] = side0[2];
			vectorPositions[vecOffset + 7] = f3;

			vectorPositions[vecOffset + 8] = P20;
			vectorPositions[vecOffset + 9] = f4;
			vectorPositions[vecOffset + 10] = P10;
			vectorPositions[vecOffset + 11] = f5;
			vectorPositions[vecOffset + 12] = P12;
			vectorPositions[vecOffset + 13] = f6;
			vectorPositions[vecOffset + 14] = P22;
			vectorPositions[vecOffset + 15] = f7;
		}
	}
};