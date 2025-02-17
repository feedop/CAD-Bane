export module multitexture;

import std;
import glm;

import <glad/glad.h>;

import shader;
import glutils;
import floodfill;
import imopen;

// <summary>
/// A class that manages an OpenGL texture with framebuffer support supporting multiple stacked draws.
/// It allows rendering intersections, performing flood fill operations, and 
/// applying morphological opening operations.
/// </summary>
export class MultiTexture
{
public:
	/// <summary>
	/// Default constructor for MultiTexture.
	/// </summary>
	MultiTexture() = default;

	/// <summary>
	/// Constructs a MultiTexture with the specified size and initializes OpenGL resources.
	/// </summary>
	/// <param name="size">The size of the texture (width and height in pixels).</param>
	MultiTexture(int size) : size(size)
	{
		data = std::vector<float>(size * size);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size, size, 0, GL_RED, GL_FLOAT, 0);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/// <summary>
	/// Destructor that cleans up OpenGL resources.
	/// </summary>
	~MultiTexture()
	{
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &texId);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}

	/// <summary>
	/// Adds intersection points to the texture using OpenGL rendering.
	/// </summary>
	/// <param name="positions">A vector of 3D positions representing the intersection.</param>
	/// <param name="shader">Pointer to the shader used for rendering.</param>
	/// <param name="tolU">Tolerance for the U coordinate.</param>
	/// <param name="tolV">Tolerance for the V coordinate.</param>
	void addIntersection(const std::vector<glm::vec3>& positions, const Shader* shader, float tolU, float tolV)
	{
		if (positions.size() <= 1)
		{
			return;
		}
		
		auto adaptedPositions = adaptPositions(positions, tolU, tolV);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, adaptedPositions.size() * sizeof(glm::vec3), &adaptedPositions[0], GL_DYNAMIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		// Render to texture	
		glViewport(0, 0, size, size);

		shader->use();
		shader->setVector("color", { 1.0f, 0.0f, 0.0f, 1.0f });
		shader->setMatrix("model", glm::mat4(1.0f));
		shader->setMatrix("view", glm::mat4(1.0f));
		shader->setMatrix("projection", glm::mat4(1.0f));
		glDrawArrays(GL_LINES, 0, adaptedPositions.size());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/// <summary>
	/// Performs a flood-fill operation starting at the given (x, y) coordinates.
	/// </summary>
	/// <param name="startX">The starting x-coordinate.</param>
	/// <param name="startY">The starting y-coordinate.</param>
	/// <param name="dummy">If true, the function returns without performing any operations.</param>
	void floodFill(int startX, int startY, bool dummy = false)
	{
		if (texId == 0)
			return;

		if (firstTime)
		{
			glBindTexture(GL_TEXTURE_2D, texId);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, &data[0]);
			firstTime = false;
		}
		

		if (dummy)
			return;

		int start = size * startY + startX;
		while (data[start] == 1.0f && start < size * size)
			start++;

		floodFillBlack(data, start % size, start / size, size);	
	}

	/// <summary>
	/// Performs morphological opening on the texture using a predefined structuring element.
	/// </summary>
	void imopen()
	{
		static const std::vector<std::vector<int>> structElem = {
			{0, 1, 0},
			{1, 1, 1},
			{0, 1, 0}
		};

		// Morphological opening
		for (int i = 0; i < 2; i++)
		{
			data = colors::imopen(data, structElem, size, size);
		}
	}

	/// <summary>
	/// Gets the texture data as a vector of floating-point values.
	/// </summary>
	/// <returns>A reference to the vector containing the texture data.</returns>
	inline const std::vector<float>& getData() const
	{	
		return data;
	}

	/// <summary>
	/// Samples the texture at a given (u, v) coordinate.
	/// </summary>
	/// <param name="u">The normalized u-coordinate (0 to 1).</param>
	/// <param name="v">The normalized v-coordinate (0 to 1).</param>
	/// <returns>The sampled value from the texture.</returns>
	float sample(float u, float v)
	{
		std::clamp(u, 0.0f, 1.0f);
		std::clamp(v, 0.0f, 1.0f);
		auto x = static_cast<int>(u * (size - 1));
		auto y = static_cast<int>(v * (size - 1));
		return data[y * size + x];
	}

private:
	int size;
	std::vector<float> data;
	unsigned int framebuffer = 0, texId = 0, VAO = 0, VBO = 0;
	bool firstTime = true;
};