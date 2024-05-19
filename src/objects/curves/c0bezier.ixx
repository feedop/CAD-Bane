export module c0bezier;

import <glad/glad.h>;
import <vector>;

import colors;
import curve;
import glutils;
import point;
import shader;

export class C0Bezier : public Curve
{
public:
	C0Bezier(const std::vector<Point*>& points) : Curve(getCurveName(), points)
	{
		genBuffers();
	}

	C0Bezier(std::initializer_list<Point*> points) : Curve(getCurveName(), points)
	{
		genBuffers();
	}

	virtual ~C0Bezier()
	{
		{
			ScopedBindArray ba(quadraticVAO);

			glBindBuffer(GL_ARRAY_BUFFER, quadraticVBO);
			glDeleteBuffers(1, &quadraticVBO);

			glDeleteVertexArrays(1, &quadraticVAO);
		}
		{
			ScopedBindArray ba(linesVAO);

			glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
			glDeleteBuffers(1, &linesVBO);

			glDeleteVertexArrays(1, &linesVAO);
		}
	}

	virtual void drawQuadratic(const Shader* shader) const override
	{
		if (positionsQuadratic.size() == 0)
			return;

		ScopedBindArray ba(quadraticVAO);
		setColor(shader);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawArrays(GL_PATCHES, 0, positionsQuadratic.size());
	}

	virtual void drawLines(const Shader* shader) const override
	{
		if (positionsLine.size() == 0)
			return;

		ScopedBindArray ba(linesVAO);
		setColor(shader);
		glDrawArrays(GL_LINES, 0, positionsLine.size());
	}

	virtual void update() override
	{
		if (!scheduledToUpdate)
			return;

		fillPositions();
		uploadPositions();
		
		scheduledToUpdate = false;
	}

	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<unsigned int>& indices) const override
	{
		MG1::BezierC0 curve;
		curve.name = getName();
		for (auto i : indices)
		{
			curve.controlPoints.emplace_back(i + 1);	
		}
		mgscene.bezierC0.push_back(curve);
	}

private:
	inline static unsigned int instanceCount = 0;

	unsigned int quadraticVAO, linesVAO = 0;
	unsigned int quadraticVBO, linesVBO = 0;

	virtual std::string getCurveName() const override
	{
		return std::format("{} {}", "C0 Bezier", instanceCount++);
	}

	virtual void genBuffers() override
	{
		glGenVertexArrays(1, &quadraticVAO);
		glGenBuffers(1, &quadraticVBO);
		glGenVertexArrays(1, &linesVAO);
		glGenBuffers(1, &linesVBO);
	}

	virtual void fillPositions() override
	{
		positions.clear();
		positionsQuadratic.clear();
		positionsLine.clear();
		auto inputSize = points.size();

		switch (inputSize)
		{
		case 0:
		case 1:
			break;
		case 2:
			for (int i = 0; i < inputSize; i++)
			{
				positionsLine.push_back(points[i]->getPosition());
			}
			break;
		case 3:
			for (int i = 0; i < inputSize; i++)
			{
				positionsQuadratic.push_back(points[i]->getPosition());
			}
			break;
		default:
			// First points
			for (int i = 0; i < degree + 1; i++)
			{
				positions.push_back(points[i]->getPosition());
			}
			positions.push_back(points[degree]->getPosition());

			int pointsProcessed = degree + 1;
			// full segments
			for (int i = 0; i < (inputSize - degree - 1) / degree; i++)
			{
				for (int j = 0; j < degree; j++)
				{
					positions.push_back(points[degree + 1 + degree * i + j]->getPosition());
					pointsProcessed++;
				}
				positions.push_back(points[degree + 1 + degree * i + degree - 1]->getPosition());
			}
			int rest = inputSize - pointsProcessed;
			switch (rest)
			{
			case 1:
				positionsLine.push_back(points[pointsProcessed - 1]->getPosition());
				positionsLine.push_back(points[pointsProcessed]->getPosition());
				break;
			case 2:
				positionsQuadratic.push_back(points[pointsProcessed - 1]->getPosition());
				positionsQuadratic.push_back(points[pointsProcessed]->getPosition());
				positionsQuadratic.push_back(points[pointsProcessed + 1]->getPosition());

				// for polygon rendering
				positions.push_back(points[pointsProcessed]->getPosition());
				positions.push_back(points[pointsProcessed + 1]->getPosition());
				break;
			}
		}
	}

	virtual void uploadPositions() override
	{
		Curve::uploadPositions();

		{
			ScopedBindArray ba(quadraticVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadraticVBO);
			if (positionsQuadratic.size() > 0)
			{
				glBufferData(GL_ARRAY_BUFFER, positionsQuadratic.size() * sizeof(glm::vec3), &positionsQuadratic[0], GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			}
			else
			{
				glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
			}
		}
		{
			ScopedBindArray ba(linesVAO);
			glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
			if (positionsLine.size() > 0)
			{
				glBufferData(GL_ARRAY_BUFFER, positionsLine.size() * sizeof(glm::vec3), &positionsLine[0], GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			}
			else
			{
				glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
			}
		}
	}

};