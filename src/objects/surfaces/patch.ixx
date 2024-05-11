export module patch;

import <glad/glad.h>;
import <memory>;
import <vector>;
import <glm/vec3.hpp>;
import <glm/vec4.hpp>;

import colors;
import drawable;
import glutils;
import point;
import shader;

export class Patch : public Drawable
{
public:

	Patch(const std::vector<Point*>& pointList)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glGenVertexArrays(1, &transposedVAO);
		glGenBuffers(1, &transposedVBO);

		points = pointList;
		positions.resize(points.size());
		positionsTransposed.resize(points.size());

		update();
	}

	virtual ~Patch()
	{
		ScopedBindArray ba(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDeleteBuffers(1, &VBO);

		glDeleteVertexArrays(1, &VAO);
	}

	virtual void draw(const Shader* shader) const override
	{
		ScopedBindArray ba(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawArrays(GL_PATCHES, 0, positions.size());
	}

	void update()
	{
		for (int i = 0; i < points.size(); i++)
		{
			positions[i] = points[i]->getPosition();
		}
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				positionsTransposed[j * 4 + i] = points[i * 4 + j]->getPosition();
			}
		}
		{
			ScopedBindArray ba(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}
		{
			ScopedBindArray tba(transposedVAO);
			glBindBuffer(GL_ARRAY_BUFFER, transposedVBO);
			glBufferData(GL_ARRAY_BUFFER, positionsTransposed.size() * sizeof(glm::vec3), &positionsTransposed[0], GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}
	}

	void drawPolygon(const Shader* shader) const
	{
		shader->setVector("color", polygonColor);
		shader->setMatrix("model", glm::mat4{ 1.0f });
		{
			ScopedBindArray ba(VAO);
			// Vertical
			for (int i = 0; i < 4; i++)
			{
				glDrawArrays(GL_LINE_STRIP, i * 4, 4);
			}
		}
		{
			ScopedBindArray tba(transposedVAO);
			// Horizontal
			for (int i = 0; i < 4; i++)
			{
				glDrawArrays(GL_LINE_STRIP, i * 4, 4);
			}
		}	
	}

private:
	inline static const glm::vec4 polygonColor = colors::orange;

	unsigned int transposedVAO = 0, transposedVBO = 0;

	std::vector<Point*> points;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> positionsTransposed;
};