export module interpolatingspline;

import std;

import <glad/glad.h>;
import <glm/vec3.hpp>;
import <glm/vec4.hpp>;

import curve;
import glutils;
import pointrenderer;
import math;

export class InterpolatingSpline : public Curve
{
public:
	InterpolatingSpline(const std::vector<Point*>& points) : Curve(getCurveName(), points)
	{}

	InterpolatingSpline(std::initializer_list<Point*> points) : Curve(getCurveName(), points)
	{}

	InterpolatingSpline(const std::string& curveName) : Curve(curveName, {})
	{}

	virtual void draw(const Shader* shader) const override
	{
		if (positions.size() == 0)
			return;

		ScopedBindArray ba(VAO);
		ScopedLineWidth lw(2.0f);
		setColor(shader);
		glPatchParameteri(GL_PATCH_VERTICES, 2);

		for (int i = 0; i < positions.size() - 1; i++)
		{
			shader->setVector("b", b[i]);
			shader->setVector("c", c[i]);
			shader->setVector("d", d[i]);
			shader->setFloat("dist", dist[i]);
			glDrawArrays(GL_PATCHES, i, 2);
		}
	}

	virtual void update() override
	{
		if (!scheduledToUpdate)
			return;

		fillPositions();
		uploadPositions();
		calculateCoefficients();

		scheduledToUpdate = false;
	}

	virtual bool isInterpolating() const override
	{
		return true;
	}

	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<unsigned int>& indices) const override
	{
		MG1::InterpolatedC2 curve;
		curve.name = getName();
		for (auto i : indices)
		{
			curve.controlPoints.emplace_back(i + 1);
		}
		mgscene.interpolatedC2.push_back(curve);
	}

protected:
	std::vector<glm::vec3> b;
	std::vector<glm::vec3> c;
	std::vector<glm::vec3> d;

	std::vector<float> dist;

	std::vector<float> alpha;
	std::vector<float> beta;
	std::vector<float> scratch;

	virtual void fillPositions() override
	{
		positions.clear();
		if (points.size() == 0)
			return;

		// Check if the points are not too close to each other
		static constexpr float eps = 1e-3;

		positions.push_back(points[0]->getPosition());
		for (int i = 1; i < points.size(); i++)
		{
			auto& pos1 = points[i - 1]->getPosition();
			auto& pos2 = points[i]->getPosition();
			if (glm::length(pos2 - pos1) > eps)
				positions.push_back(pos2);
		}
	}

	void calculateCoefficients()
	{
		auto N = positions.size();
		auto segments = N - 1;
		if (b.size() != N)
		{
			b.resize(N);
			c.resize(N + 1);
			d.resize(N);

			if (N > 1)
			{
				dist.resize(segments);
				alpha.resize(segments);
				beta.resize(segments);
				scratch.resize(segments - 1);
			}
		}

		switch (N)
		{
		case 0:
			return;
		case 1:
			b[0] = c[0] = d[0] = { 0,0,0 };
			return;
		case 2:
			auto diff = positions[1] - positions[0];
			b[0] = glm::normalize(diff);
			c[0] = d[0] = { 0,0,0 };
			dist[0] = glm::length(diff);
			return;
		}

		// Fill dist array
		for (int i = 0; i < segments; i++)
		{
			dist[i] = glm::length(positions[i + 1] - positions[i]);
		}

		c[0] = c[N] = { 0,0,0 };

		for (int i = 1; i < segments; i++)
		{
			float denom = dist[i - 1] + dist[i];
			alpha[i - 1] = dist[i - 1] / denom;
			beta[i - 1] = dist[i] / denom;
			// R[i]
			c[i] = 3.0f * ((positions[i + 1] - positions[i]) / dist[i] - (positions[i] - positions[i - 1]) / dist[i - 1]) / denom;
		}

		// Solve the system of equations
		solve(segments - 1);

		// Fill remaining polynomial coefficients
		for (int i = 1; i < N; i++)
		{
			d[i - 1] = (c[i] - c[i - 1]) / (3 * dist[i - 1]);
		}
		for (int i = 1; i < N; i++)
		{
			auto di = dist[i - 1];
			b[i - 1] = (positions[i] - positions[i - 1] - c[i - 1] * di * di - d[i - 1] * di * di * di) / di;
		}
	}

private:
	inline static unsigned int instanceCount = 0;

	virtual std::string getCurveName() const override
	{
		return std::format("{} {}", "Interpolating spline", instanceCount++);
	}

	void solve(int X)
	{
		// Tridiagonal matrix algorithm (Thomas algorithm)
		scratch[0] = beta[0] / 2;
		c[1] = c[1] * 0.5f;

		// loop from 1 to X - 1 inclusive
		for (int ix = 1; ix < X; ix++)
		{
			if (ix < X - 1)
			{
				scratch[ix] = beta[ix] / (2 - alpha[ix] * scratch[ix - 1]);
			}
			c[ix + 1] = (c[ix + 1] - alpha[ix] * c[ix]) / (2 - alpha[ix] * scratch[ix - 1]);
		}

		// loop from X - 2 to 0 inclusive
		for (int ix = X - 2; ix >= 0; ix--)
			c[ix + 1] -= scratch[ix] * c[ix + 2];
	}
};