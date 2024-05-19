export module c2bezier;

import <glad/glad.h>;
import <glm/vec3.hpp>;
import <glm/vec4.hpp>;

import colors;
import curve;
import glutils;
import pointrenderer;
import math;
import virtualpoint;

export class C2Bezier : public Curve
{
public:
	C2Bezier(const std::vector<Point*>& points) : Curve(getCurveName(), points)
	{
		genBuffers();
	}

	C2Bezier(std::initializer_list<Point*> points) : Curve(getCurveName(), points)
	{
		genBuffers();
	}

	virtual ~C2Bezier()
	{
		ScopedBindArray ba(bSplineVAO);

		glBindBuffer(GL_ARRAY_BUFFER, bSplineVBO);
		glDeleteBuffers(1, &bSplineVBO);

		glDeleteVertexArrays(1, &bSplineVAO);
	}

	virtual inline void drawAdditionalPoints(const Shader* shader) const override
	{
		pointRenderer.draw(shader);
	}

	virtual void drawPolygon(const Shader* shader) const
	{
		shader->setVector("color", bSplineColor);
		shader->setMatrix("model", glm::mat4{ 1.0f });
		ScopedBindArray ba(bSplineVAO);
		glDrawArrays(GL_LINE_STRIP, 0, bSplinePosiitons.size());

		Curve::drawPolygon(shader);
	}

	virtual void update() override
	{
		if (!scheduledToUpdate)
			return;

		if (updateInvoker)
		{
			bezierToBSpline();
			updateInvoker = nullptr;
		}

		bSplineToBezier();
		fillPositions();
		uploadPositions();
		updateRenderer();

		scheduledToUpdate = false;
	}

	virtual inline const std::vector<std::unique_ptr<Point>>& getVirtualPoints() const override
	{
		return virtualPoints;
	}

	virtual inline void updateRenderer() override
	{
		pointRenderer.updateSoft(virtualPoints);
	}

	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<unsigned int>& indices) const override
	{
		MG1::BezierC2 curve;
		curve.name = getName();
		for (auto i : indices)
		{
			curve.controlPoints.emplace_back(i + 1);
		}
		mgscene.bezierC2.push_back(curve);
	}

private:
	inline static unsigned int instanceCount = 0;

	inline static const glm::vec4 bSplineColor = colors::navy;

	std::vector<std::unique_ptr<Point>> virtualPoints;
	PointRenderer pointRenderer;

	int lastPointsSize = -1;
	unsigned int bSplineVAO = 0, bSplineVBO = 0;
	std::vector<glm::vec3> bSplinePosiitons;

	virtual std::string getCurveName() const override
	{
		return std::format("{} {}", "C2 Bezier", instanceCount++);
	}

	virtual void genBuffers() override
	{
		glGenVertexArrays(1, &bSplineVAO);
		glGenBuffers(1, &bSplineVBO);
	}

	virtual void fillPositions() override
	{
		positions.clear();
		auto inputSize = virtualPoints.size();

		if (inputSize >=4)
		{
			positions.push_back(virtualPoints[0]->getPosition());

			int pointsProcessed = degree + 1;
			// full segments
			for (int i = 0; i < (inputSize) / degree; i++)
			{
				for (int j = 0; j < degree; j++)
				{
					positions.push_back(virtualPoints[1 + degree * i + j]->getPosition());
				}
				positions.push_back(virtualPoints[1 + degree * i + degree - 1]->getPosition());
			}
		}

		bSplinePosiitons.clear();
		for (auto&& point : points)
		{
			bSplinePosiitons.push_back(point->getPosition());
		}

		ScopedBindArray ba(bSplineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, bSplineVBO);
		glBufferData(GL_ARRAY_BUFFER, bSplinePosiitons.size() * sizeof(glm::vec3), &bSplinePosiitons[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	}

	void bSplineToBezier()
	{
		bool hardUpdate = points.size() != lastPointsSize;
		auto deBoorPointCount = points.size();

		if (deBoorPointCount >= 4)
		{
			int m = deBoorPointCount - 3;
			std::vector<glm::vec3> f;
			std::vector<glm::vec3> g;
			std::vector<glm::vec3> e;

			f.push_back(points[0]->getPosition());
			g.push_back(0.5f * points[0]->getPosition() + 0.5f * points[1]->getPosition());

			for (int i = 1; i <= m + 1; i++)
			{
				f.push_back(2.0f / 3 * points[i]->getPosition() + 1.0f / 3 * points[i + 1]->getPosition());
				g.push_back(1.0f / 3 * points[i]->getPosition() + 2.0f / 3 * points[i + 1]->getPosition());
			}
			e.push_back(points[0]->getPosition());
			for (int i = 0; i <= m; i++)
			{
				e.push_back(0.5f * g[i] + 0.5f * f[i + 1]);
			}

			if (hardUpdate)
			{
				virtualPoints.clear();
				for (int i = 1; i <= m; i++)
				{
					virtualPoints.emplace_back(new VirtualPoint(e[i]));
					virtualPoints.emplace_back(new VirtualPoint(f[i]));
					virtualPoints.emplace_back(new VirtualPoint(g[i]));
				}
				virtualPoints.emplace_back(new VirtualPoint(e[m + 1]));
			}
			else
			{
				for (int i = 0; i < (virtualPoints.size() - 1) / 3; i++)
				{
					virtualPoints[3 * i]->setPosition(e[i + 1]);
					virtualPoints[3 * i + 1]->setPosition(f[i + 1]);
					virtualPoints[3 * i + 2]->setPosition(g[i + 1]);
				}
				virtualPoints[virtualPoints.size() - 1]->setPosition(e[m + 1]);
			}
		}

		if (hardUpdate)
		{
			for (auto& point : virtualPoints)
			{
				point->attach(this);
			}
		}

		lastPointsSize = points.size();
	}

	void bezierToBSpline()
	{
		auto& invokerTranslation = updateInvoker->getLastTranslation();
		auto& invokerPosition = updateInvoker->getPosition();
		auto it = std::find_if(virtualPoints.begin(), virtualPoints.end(), [&](const auto& vp) { return vp.get() == updateInvoker; });
		int index = it - virtualPoints.begin();
		int indexInBezier = index % 3;

		int segment = index / 3;
		int toMove = indexInBezier < 2 ? segment + 1 : segment + 2;	

		float coeff;
		switch (indexInBezier)
		{
		case 0:
			coeff = 1.5f;
			break;
		case 1:
			coeff = 1.5f;
			break;
		case 2:
			coeff = 1.5f;
			break;
		}
		points[toMove]->translate(invokerTranslation * coeff);
	}
};