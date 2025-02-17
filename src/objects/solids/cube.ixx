export module cube;

import std;
import glm;

import shader;
import texturedobject;

/// <summary>
/// Represents a 3D cube object with texture mapping. 
/// The cube is defined by 8 vertices and 12 triangles to form a closed cube in 3D space.
/// </summary>
export class Cube : public TexturedObject
{
public:
	/// <summary>
	/// Constructor for the Cube class that initializes the cube object with vertices and indices to form a textured cube.
	/// </summary>
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