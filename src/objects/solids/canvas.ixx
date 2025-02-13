export module canvas;

import std;
import glm;

import shader;

import texturedobject;

export class Canvas : public TexturedObject
{
public:
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