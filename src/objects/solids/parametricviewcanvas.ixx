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

export class ParametricViewCanvas : public Canvas
{
public:
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

	virtual ~ParametricViewCanvas()
	{
		glDeleteTextures(1, &texId);
		glDeleteFramebuffers(1, &framebuffer);
	}

	virtual void draw(const Shader* shader) const override
	{
		glBindTexture(GL_TEXTURE_2D, texId);
		Canvas::draw(shader);
	}

private:
	unsigned int framebuffer = 0;
	unsigned int texId = 0;

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