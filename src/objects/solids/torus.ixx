export module torus;

import std;
import glm;

import <glad/glad.h>;

import <Serializer/Serializer.h>;

import glutils;
import clickable;
import math;
import parametric;
import selectable;
import solidobject;
import shader;
import mg1utils;

/// <summary>
/// A class representing a 3D torus object, derived from <see cref="SolidObject"/>, <see cref="Selectable"/>, 
/// <see cref="Clickable"/>, and <see cref="Parametric"/>. It provides functionality to calculate, render, 
/// and interact with a torus object in 3D space, including texture handling, parameter evaluation, and selection.
/// </summary>
export class Torus : public SolidObject, public Selectable, public Clickable, public Parametric
{
	/// <summary>
	/// Structure representing a vertex with translation and texture coordinates.
	/// </summary>
	struct Vertex
	{
		glm::vec3 translation;
		glm::vec2 texCoords;
	};

public:
	// <summary>
	/// Constructor for creating a torus object with optional parameters.
	/// </summary>
	/// <param name="translation">The initial position of the torus in 3D space (default: {0, 0, 0}).</param>
	/// <param name="R">The large radius of the torus (default: 1.0f).</param>
	/// <param name="r">The small radius of the torus (default: 0.2f).</param>
	/// <param name="scale">The scale of the torus (default: {1, 1, 1}).</param>
	/// <param name="rotation">The rotation of the torus (default: no rotation).</param>
	/// <param name="majorPoints">The number of segments around the major circle (default: 40).</param>
	/// <param name="minorPoints">The number of segments around the minor circle (default: 20).</param>
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

	/// <summary>
	/// Deserializes a torus from a MG1::Torus.
	/// </summary>
	/// <param name="other">The other torus object to copy from.</param>
	Torus(const MG1::Torus& other) : Torus(toGLM(other.position), other.largeRadius, other.smallRadius, toGLM(other.scale),
										   math::Quat(toGLM(other.rotation)),
										   other.samples.x, other.samples.y)
	{
		if (!other.name.empty())
			setName(other.name);
	}

	/// <summary>
	/// Serializes the current torus object to an MG1::Torus" object.
	/// </summary>
	/// <returns>The equivalent MG1::Torus object.</returns>
	operator MG1::Torus() const
	{
		MG1::Torus t;
		t.position = toMG1(position);
		t.largeRadius = R;
		t.smallRadius = r;
		t.scale = toMG1(scale);
		t.rotation = toMG1(rotation.rpy());
		t.samples.x = majorPoints;
		t.samples.y = minorPoints;
		return t;
	}

	/// <summary>
	/// Draws the torus using the provided shader. 
	/// </summary>
	/// <param name="shader">The shader to use for rendering the torus.</param>
	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		setColor(shader);
		Parametric::uploadTrimTexture(shader);
		SolidObject::draw(shader);
		glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	}

	/// <summary>
	/// Determines if the given 3D coordinate is inside the torus object.
	/// </summary>
	/// <param name="coord">The 3D coordinate to check.</param>
	/// <returns>True if the coordinate is inside the torus, false otherwise.</returns>
	virtual bool isCoordInObject(const glm::vec3& coord) const override
	{
		auto diff = coord - position;
		// From torus equation
		float left = std::powf(R - std::sqrtf(diff.x * diff.x + diff.y * diff.y), 2) + coord.z * coord.z;
		return (left <= r * r);
	}

	/// <summary>
	/// Evaluates the surface at the given parameters (u, v) and with an optional tool radius.
	/// </summary>
	/// <param name="u">U parameter for the surface evaluation.</param>
	/// <param name="v">V parameter for the surface evaluation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D point corresponding to the (u, v) parameter on the surface.</returns>
	virtual glm::vec3 evaluate(float u, float v, float toolRadius = 0.0f) const override
	{
		u *= 2 * math::pi;
		v *= 2 * math::pi;
		float totalRadius = R + r * std::cos(u);

		float x = totalRadius * std::cos(v);
		float z = totalRadius * std::sin(v);
		float y = r * std::sin(u);

		return model * glm::vec4{ x, y, z, 1 };
	}

	/// <summary>
	/// Calculates the derivative of the surface in the U direction at the given parameters (u, v).
	/// </summary>
	/// <param name="u">U parameter for the derivative calculation.</param>
	/// <param name="v">V parameter for the derivative calculation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D vector representing the derivative in the U direction.</returns>
	virtual glm::vec3 derivativeU(float u, float v, float toolRadius = 0.0f) const override
	{
		u *= 2 * math::pi;
		v *= 2 * math::pi;
		float totalRadius = -r * std::sin(u);

		float x = totalRadius * std::cos(v);
		float z = totalRadius * std::sin(v);
		float y = r * std::cos(u);

		return { x, y, z};
	}

	/// <summary>
	/// Calculates the derivative of the surface in the V direction at the given parameters (u, v).
	/// </summary>
	/// <param name="u">U parameter for the derivative calculation.</param>
	/// <param name="v">V parameter for the derivative calculation.</param>
	/// <param name="toolRadius">The surface's offset along its normal vector.</param>
	/// <returns>The 3D vector representing the derivative in the V direction.</returns>
	virtual glm::vec3 derivativeV(float u, float v, float toolRadius = 0.0f) const override
	{
		u *= 2 * math::pi;
		v *= 2 * math::pi;
		float totalRadius = R + r * std::cos(u);

		float x = totalRadius * -std::sin(v);
		float z = totalRadius * std::cos(v);
		float y = 0;

		return { x, y, z};
	}

	/// <summary>
	/// Gets the range of the parameter u or v, which is always 2pi for a torus.
	/// </summary>
	/// <returns>The range (2pi) of the u and v parameters.</returns>
	virtual constexpr float getRange() const override
	{
		return 2.0f * math::pi;
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

	/// <summary>
	/// Calculates the vertices and indices for the torus shape based on the parameters.
	/// </summary>
	void calculateTorus()
	{
		vertices.clear();
		indices.clear();

		for (int i = 0; i < minorPoints; i++)
		{
			float majorCircleAngle = i * 2 * math::pi / minorPoints;
			float majorCircleAngleSin = std::sin(majorCircleAngle);
			float totalRadius = R + r * std::cos(majorCircleAngle);

			for (int j = 0; j < majorPoints; j++)
			{
				float minorCircleAngle = j * 2 * math::pi / majorPoints;
				
				float x = totalRadius * std::cos(minorCircleAngle);
				float z = totalRadius * std::sin(minorCircleAngle);
				float y = r * majorCircleAngleSin;

				vertices.emplace_back(glm::vec3{ x, y, z }, glm::vec2{ majorCircleAngle / (2 * math::pi),  minorCircleAngle / (2 * math::pi) });

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

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// texture coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	}
};