export module parametricviewcanvas;

import std;

import <glad/glad.h>;

import <glm/mat4x4.hpp>;
import <glm/vec2.hpp>;

import canvas;
import glutils;
import math;
import shader;

inline constexpr int size = 512;

inline int toIndex(int x, int y)
{
	return y * size + x;
}

inline bool isValid(int x, int y, int rows, int cols)
{
	return (x >= 0 && x < rows && y >= 0 && y < cols);
}

inline bool isSkippable(const std::vector<glm::vec3>& positions, int i)
{
	return std::abs(positions[i - 1].x - positions[i].x) > 0.9f ||
		std::abs(positions[i - 1].y - positions[i].y) > 0.9f;
}

glm::vec3 endAtEdge(const glm::vec3& pos)
{
	static constexpr float tol = 5e-2;
	glm::vec3 newPos = pos;
	if (newPos.x < -1.0f + tol)
		newPos.x = -1.2f;
	else if (newPos.x > 1.0f - tol)
		newPos.x = 1.2f;

	if (newPos.y < -1.0f + tol)
		newPos.y = -1.2f;
	else if (newPos.y > 1.0f - tol)
		newPos.y = 1.2f;

	return newPos;
}

std::vector<glm::vec3> adaptPositions(const std::vector<glm::vec3>& positions)
{
	std::vector<glm::vec3> ret;
	for (int i = 1; i < positions.size(); i++)
	{
		if (isSkippable(positions, i))
			continue;

		ret.push_back(endAtEdge(positions[i - 1]));
		ret.push_back(endAtEdge(positions[i]));
	}
	return ret;
}

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

		floodfill(data, start % size, start / size);

		unsigned int trimmingTex;
		glGenTextures(1, &trimmingTex);
		glBindTexture(GL_TEXTURE_2D, trimmingTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_FLOAT, &data[0]);

		return trimmingTex;
	}

	void floodfill(std::vector<glm::vec4>& image, int x, int y)
	{
		auto initialColor = image[toIndex(x, y)];
		const glm::vec4 newColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		// Directions array for 4-directional movement (up, down, left, right)
		std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

		// Queue to store the points to process
		std::queue<std::pair<int, int>> q;
		q.push({ x, y });
		image[toIndex(x, y)] = newColor; // Set the starting point to the new color

		// BFS loop
		while (!q.empty())
		{
			auto [x, y] = q.front();
			q.pop();

			// Process all 4 neighbors
			for (const auto& dir : directions)
			{
				int newX = x + dir.first;
				int newY = y + dir.second;

				// If the new coordinates are valid and the cell has the initial color
				if (isValid(newX, newY, size, size) && image[toIndex(newX, newY)] == initialColor)
				{
					image[toIndex(newX, newY)] = newColor;  // Change color
					q.push({ newX, newY });  // Add to queue for further processing
				}
			}
		}
	}
};