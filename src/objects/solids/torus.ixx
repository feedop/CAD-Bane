export module torus;

import <glad/glad.h>;
import <cmath>;
import <format>;
import <vector>;

import <glm/vec4.hpp>;
import <Serializer/Serializer.h>;

import glutils;
import clickable;
import math;
import selectable;
import solidobject;
import shader;
import vec3conversion;

export class Torus : public SolidObject, public Selectable, public Clickable
{
	struct Vertex
	{
		glm::vec3 translation;
	};

public:
	Torus(const glm::vec3& translation = glm::vec3{ 0.0f, 0.0f, 0.0f }, float R = 1.0f, float r = 0.2f,
		  const glm::vec3& scale = { 1.0f, 1.0f, 1.0f }, const math::Quat& rotation = math::Quat{},
		  int majorPoints = 40, int minorPoints = 20) :
		SolidObject(translation, rotation, scale), Selectable(std::format("Torus {}", instanceCount++)),
		R(R), r(r), majorPoints(majorPoints), minorPoints(minorPoints)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		calculateTorus();
	}

	Torus(const MG1::Torus& other) : Torus(toGLM(other.position), other.largeRadius, other.smallRadius, toGLM(other.scale),
										   math::Quat(math::pi / 180.0f * toGLM(other.rotation)), other.samples.x, other.samples.y)
	{
		if (!other.name.empty())
			setName(other.name);
	}

	operator MG1::Torus() const
	{
		MG1::Torus t;
		t.position = toMG1(position);
		t.largeRadius = R;
		t.smallRadius = r;
		t.scale = toMG1(scale);
		t.rotation = toMG1(180.0f / math::pi * rotation.rpy());
		t.samples.x = majorPoints;
		t.samples.y = minorPoints;
		return t;
	}

	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		setColor(shader);
		SolidObject::draw(shader);
		glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
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

	float R;
	float r;
	int majorPoints = 40;
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
		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
					 &indices[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	}
};