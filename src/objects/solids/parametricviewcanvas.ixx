export module parametricviewcanvas;

import std;
import glm;

import <glad/glad.h>;

import canvas;
import glutils;
import math;
import shader;
import floodfill;

inline constexpr int size = 512;

/// <summary>
/// A canvas that renders a parametric view of a 3D object to a texture, using a framebuffer and shader. 
/// It adapts a set of 3D positions into a texture, allowing for parametric visualization of a surface or curve.
/// </summary>
export class ParametricViewCanvas : public Canvas
{
public:
	/// <summary>
	/// Constructor that initializes the canvas by setting up a framebuffer, texture, and rendering a parametric view 
	/// of a 3D object. The texture is created and the positions of the 3D points are adapted and drawn to the texture 
	/// using the provided shader.
	/// </summary>
	/// <param name="positions">A vector of 3D positions representing the parametric curve or surface.</param>
	/// <param name="shader">The shader used to render the positions to the texture.</param>
	/// <param name="texture">The texture that will hold the rendered parametric view.</param>
	ParametricViewCanvas(const std::vector<glm::vec3>& positions, const Shader* shader, unsigned int& texture) : Canvas()
	{
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_FLOAT, 0);

		if (positions.size() <= 1)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return;
		}

		auto adaptedPositions = adaptPositions(positions);

		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, adaptedPositions.size() * sizeof(glm::vec3), &adaptedPositions[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		// Render to texture
		glViewport(0, 0, size, size);

		shader->use();
		shader->setVector("color", {1.0f, 1.0f, 1.0f, 1.0f});
		shader->setMatrix("model", glm::mat4(1.0f));
		shader->setMatrix("view", glm::mat4(1.0f));
		shader->setMatrix("projection", glm::mat4(1.0f));
		glDrawArrays(GL_LINES, 0, adaptedPositions.size());

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		texture = createTrimmingTexture();
	}

	/// <summary>
	/// Destructor that cleans up the OpenGL resources used by the framebuffer and texture.
	/// </summary>
	virtual ~ParametricViewCanvas()
	{
		glDeleteTextures(1, &texId);
		glDeleteFramebuffers(1, &framebuffer);
	}

	/// <summary>
	/// Draws the parametric view texture onto the canvas using the specified shader.
	/// </summary>
	/// <param name="shader">The shader used for rendering the canvas.</param>
	virtual void draw(const Shader* shader) const override
	{
		glBindTexture(GL_TEXTURE_2D, texId);
		Canvas::draw(shader);
	}

private:
	unsigned int framebuffer = 0;
	unsigned int texId = 0;

	// <summary>
	/// Creates a trimming texture by reading the texture data, applying a flood fill algorithm, 
	/// and generating a new texture with the filled data.
	/// </summary>
	/// <returns>The generated trimming texture ID.</returns>
	unsigned int createTrimmingTexture()
	{
		std::vector<glm::vec4> data(size * size);
		glBindTexture(GL_TEXTURE_2D, texId);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &data[0]);

		// Flood fill
		int start = size * size/2 + size/2;
		while (data[start].x == 1.0f && start < size * size)
			start++;

		floodFillColor(data, start % size, start / size, size);

		unsigned int trimmingTex;
		glGenTextures(1, &trimmingTex);
		glBindTexture(GL_TEXTURE_2D, trimmingTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_FLOAT, &data[0]);

		return trimmingTex;
	}
};