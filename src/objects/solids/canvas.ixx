export module canvas;

import std;
import glm;

import shader;

import texturedobject;

// <summary>
/// Represents a 2D canvas object in 3D space, typically used to display textures or images. 
/// The canvas is a quadrilateral defined by four vertices and two triangles for its surface.
/// </summary>
export class Canvas : public TexturedObject
{
public:
	/// <summary>
	/// Constructor for the Canvas class that initializes the canvas object with vertices and indices for a quadrilateral.
	/// </summary>
	Canvas() : TexturedObject(createVertices(), createIndices())
	{}

private:
	static std::vector<Vertex> createVertices()
	{
		return {
			{
				{-1.0f, 1.0f, 0.0f},
				{ 0.0f, 0.0f }
			},
			{
				{1.0f, 1.0f, 0.0f},
				{0.0f, 1.0f}
			},
			{
				{-1.0f, -1.0f, 0.0f},
				{1.0f, 0.0f}
			},
			{
				{1.0f, -1.0f, 0.0f},
				{1.0f, 1.0f}
			}
		};
	}

	static std::vector<unsigned int> createIndices()
	{
		return {
			0, 1, 2,
			1, 3, 2
		};
	}
};