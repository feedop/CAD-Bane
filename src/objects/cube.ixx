export module cube;

import <vector>;

import <glm/vec2.hpp>;
import <glm/vec3.hpp>;

import shader;
import texturedobject;

export class Cube : public TexturedObject
{
public:
	Cube() : TexturedObject(createVertices(), createIndices())
	{}

private:
	static std::vector<Vertex> createVertices()
	{
		return {
			{
				{-1.0f, 1.0f, 1.0f},
				{ 0.0f, 0.0f }
			},
			{
				{1.0f, 1.0f, 1.0f},
				{0.0f, 1.0f}
			},
			{
				{-1.0f, -1.0f, 1.0f},
				{1.0f, 0.0f}
			},
			{
				{1.0f, -1.0f, 1.0f},
				{1.0f, 1.0f}
			},
			{
				{-1.0f, 1.0f, -1.0f},
				{ 0.0f, 0.0f }
			},
			{
				{1.0f, 1.0f, -1.0f},
				{0.0f, 1.0f}
			},
			{
				{-1.0f, -1.0f, -1.0f},
				{1.0f, 0.0f}
			},
			{
				{1.0f, -1.0f, -1.0f},
				{1.0f, 1.0f}
			}
		};
	}

	static std::vector<unsigned int> createIndices()
	{
		return {
			0, 1, 2,
			1, 3, 2,
			0, 5, 3,
			5, 7, 3,
			5, 4, 7,
			4, 6, 7,
			4, 0, 6,
			0, 2, 6,
			4, 5, 0,
			5, 6, 0,
			2, 3, 6,
			3, 7, 6
		};
	}
};