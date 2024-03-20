export module torus;

import <glad/glad.h>;
import <cmath>;
import <format>;
import <vector>;

import <glm/vec4.hpp>;

import math;
import selectableobject;
import shader;

export class Torus : public SelectableObject
{
	struct Vertex
	{
		glm::vec3 translation;
	};

public:
	Torus(const glm::vec3& translation = glm::vec3{ 0.0f, 0.0f, 0.0f }) : SelectableObject(translation)
	{
		name = std::format("Torus {}", instanceCount++);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		
		calculateTorus();
	}

	virtual void draw(const Shader* shader) const override
	{
		SelectableObject::draw(shader);
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	virtual bool isCoordInObject(const glm::vec3& coord) const override
	{
		auto diff = coord - position;
		// From torus equation
		float left = std::powf(R - std::sqrtf(diff.x * diff.x + diff.y * diff.y), 2) + coord.z * coord.z;
		return (left <= r * r);
	}

private:
	friend class GuiController;
	inline static unsigned int instanceCount = 0;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	float R = 0.5f;
	float r = 0.1f;
	int majorPoints = 30;
	int minorPoints = 20;

	void calculateTorus()
	{
		vertices.clear();
		indices.clear();

		for (int i = 0; i < minorPoints; i++)
		{
			float majorCircleAngle = i * 2 * math::pi / minorPoints;
			float majorCircleAngleCos = std::cos(majorCircleAngle);
			float majorCircleAngleSin = std::sin(majorCircleAngle);
			float totalRadius = R + r * std::cos(majorCircleAngle);

			for (int j = 0; j < majorPoints; j++)
			{
				float minorCircleAngle = j * 2 * math::pi / majorPoints;
				
				float x = totalRadius * std::cos(minorCircleAngle);
				float y = totalRadius * std::sin(minorCircleAngle);
				float z = r * majorCircleAngleSin;

				vertices.emplace_back(glm::vec3{ x, y, z });

				indices.push_back(i * majorPoints + j);
				if (j == majorPoints - 1) [[unlikely]]
					indices.push_back(i * majorPoints);
				else [[likely]]
					indices.push_back(i * majorPoints + j + 1);

				indices.push_back(i * majorPoints + j);
				if (i == minorPoints - 1) [[unlikely]]
					indices.push_back(j);
				else [[likely]]
					indices.push_back((i + 1) * majorPoints + j);
			}
		}

		// Copy data to gpu
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
					 &indices[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glBindVertexArray(0);
	}
};