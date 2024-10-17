export module intersections;


import std;

import <glm/mat4x4.hpp>;
import <glm/vec4.hpp>;

import intersectioncurve;
import parametric;
import math;

inline bool isnan(const glm::vec3& X)
{
	return glm::isnan(X.x) ||
		glm::isnan(X.y) ||
		glm::isnan(X.z);
}

inline bool isnan(const glm::vec4& X)
{
	return glm::isnan(X.x) ||
		glm::isnan(X.y) ||
		glm::isnan(X.z) ||
		glm::isnan(X.w);
}

inline bool eq(auto v1, auto v2)
{
	return std::abs(v1 - v2) <= math::eps;
}

void endCurve(const Parametric* surface1, const Parametric* surface2, std::vector<glm::vec3>& positions, std::vector<glm::vec4>& params, float d, const glm::vec4& wrap)
{
	auto& X = params.back();
	auto& pos = positions.back();
	if (!wrap.x)
	{
		if (glm::length(pos - surface1->evaluate(0.0f, X.y)) <= d)
		{
			glm::vec4 newX{ 0.0f, X.y, X.z, X.w };
			params.push_back(newX);
			positions.push_back(surface1->evaluate(newX.x, newX.y));
		}
		if (glm::length(pos - surface1->evaluate(1.0f, X.y)) <= d)
		{
			glm::vec4 newX{ 1.0f, X.y, X.z, X.w };
			params.push_back(newX);
			positions.push_back(surface1->evaluate(newX.x, newX.y));
		}
	}
	else if (!wrap.y)
	{
		if (glm::length(pos - surface1->evaluate(X.x, 0.0f)) <= d)
		{
			glm::vec4 newX{ X.x, 0.0f, X.z, X.w };
			params.push_back(newX);
			positions.push_back(surface1->evaluate(newX.x, newX.y));
		}
		if (glm::length(pos - surface1->evaluate(X.x, 1.0f)) <= d)
		{
			glm::vec4 newX{ X.x, 1.0f, X.z, X.w };
			params.push_back(newX);
			positions.push_back(surface1->evaluate(newX.x, newX.y));
		}
	}
	else if (!wrap.z)
	{
		if (glm::length(pos - surface2->evaluate(0.0f, X.w)) <= d)
		{
			glm::vec4 newX{ X.x, X.y, 0.0f, X.w };
			params.push_back(newX);
			positions.push_back(surface2->evaluate(newX.x, newX.y));
		}
		if (glm::length(pos - surface2->evaluate(1.0f, X.w)) <= d)
		{
			glm::vec4 newX{ X.x, X.y, 1.0f, X.w };
			params.push_back(newX);
			positions.push_back(surface2->evaluate(newX.x, newX.y));
		}
	}
	else if (!wrap.w)
	{
		if (glm::length(pos - surface2->evaluate(X.z, 0.0f)) <= d)
		{
			glm::vec4 newX{ X.x, X.y, X.z, 0.0f };
			params.push_back(newX);
			positions.push_back(surface2->evaluate(newX.x, newX.y));
		}
		if (glm::length(pos - surface2->evaluate(X.z, 1.0f)) <= d)
		{
			glm::vec4 newX{ X.x, X.y, X.z, 1.0f };
			params.push_back(newX);
			positions.push_back(surface2->evaluate(newX.x, newX.y));
		}
	}
}

void cap(float& value, bool wrap, float lowerLimit = 0.0f, float upperLimit = 1.0f)
{
	if (wrap)
	{
		while (value < lowerLimit)
			//value += upperLimit - lowerLimit;
			value = upperLimit;// -math::eps;
		while (value > upperLimit)
			//value -= upperLimit - lowerLimit;
			value = lowerLimit;// +math::eps;
	}
	else
	{
		if (value < lowerLimit)
			value = lowerLimit;
		else if (value > upperLimit)
			value = upperLimit;
	}
}

void cap(glm::vec4& vec, const glm::vec4& wrap)
{
	cap(vec.x, wrap.x);
	cap(vec.y, wrap.y);
	cap(vec.z, wrap.z);
	cap(vec.w, wrap.w);
}

glm::vec4 calculateGradient(const glm::vec4 X, const Parametric* surface1, const Parametric* surface2)
{
	auto ev1 = surface1->evaluate(X.x, X.y);
	auto ev2 = surface2->evaluate(X.z, X.w);
	auto diff = ev1 - ev2;

	auto du1 = surface1->derivativeU(X.x, X.y);
	auto dv1 = surface1->derivativeV(X.x, X.y);

	auto du2 = surface2->derivativeU(X.z, X.w);
	auto dv2 = surface2->derivativeV(X.z, X.w);

	return 2.0f * glm::vec4{
		glm::dot(diff, du1),
		glm::dot(diff, dv1),
		glm::dot(-diff, du2),
		glm::dot(-diff, dv2)
	};
}

glm::vec4 conjugateGradientAlgorithm(const Parametric* surface1, const Parametric* surface2, glm::vec4 X, const glm::vec4& wrap)
{
	static constexpr int maxIters = 2500;
	float cgStep = 1e-4;

	float dist = std::numeric_limits<float>::max();

	int i = 0;
	while (dist > math::eps && i < maxIters && cgStep > 1e-6)
	{
		auto gradient = calculateGradient(X, surface1, surface2);
		X -= gradient * cgStep;
		cap(X, wrap);
		float newDist = glm::length(surface1->evaluate(X.x, X.y) - surface2->evaluate(X.z, X.w));
		if (newDist >= dist)
			cgStep /= 2;

		dist = newDist;
		i++;
	}

	return X;
}

glm::vec4 conjugateGradientInitialValue(const Parametric* surface1, const Parametric* surface2, const glm::vec3& startingPointLocation, const glm::vec4& wrap)
{
	
	static constexpr float step = 5e-3;
	
	// Find params on both surfaces for the starting point
	float bestUL = 0;
	float bestVL = 0;
	float dist = std::numeric_limits<float>::max();
	for (float u = 0; u <= 1; u += step)
	{
		for (float v = 0; v <= 1; v += step)
		{
			float newDist = glm::length(startingPointLocation - surface1->evaluate(u, v));
			if (newDist < dist)
			{
				dist = newDist;
				bestUL = u;
				bestVL = v;
			}
		}
	}

	float bestUR = 0;
	float bestVR = 0;
	dist = std::numeric_limits<float>::max();
	for (float u = 0; u <= 1; u += step)
	{
		for (float v = 0; v <= 1; v += step)
		{
			float newDist = glm::length(startingPointLocation - surface2->evaluate(u, v));
			if (newDist < dist)
			{
				dist = newDist;
				bestUR = u;
				bestVR = v;
			}
		}
	}

	return conjugateGradientAlgorithm(surface1, surface2, { bestUL, bestVL, bestUR, bestVR }, wrap);
}

glm::vec4 conjugateGradientSelf(const Parametric* surface, const glm::vec3& startingPointLocation, const glm::vec4& wrap)
{
	static constexpr float step = 5e-3;
	static constexpr float penaltyMod = 1.0F;

	// Find closest point
	float bestUL = 0;
	float bestVL = 0;
	float dist = std::numeric_limits<float>::max();
	for (float u = 0; u <= 1; u += step)
	{
		for (float v = 0; v <= 1; v += step)
		{
			float newDist = glm::length(startingPointLocation - surface->evaluate(u, v));
			if (newDist < dist)
			{
				dist = newDist;
				bestUL = u;
				bestVL = v;
			}
		}
	}

	auto closestPoint = surface->evaluate(bestUL, bestVL);

	// Find a point that's furthest from previously found
	float bestUR = 0;
	float bestVR = 0;
	dist = std::numeric_limits<float>::max();
	for (float u = 0; u <= 1; u += step)
	{
		for (float v = 0; v <= 1; v += step)
		{
			auto point = surface->evaluate(u, v);
			float newDist = glm::length(startingPointLocation - point) - penaltyMod * glm::length(glm::vec2(bestUL, bestVL) - glm::vec2(u, v));
			if (newDist < dist)
			{
				dist = newDist;
				bestUR = u;
				bestVR = v;
			}
		}
	}

	return conjugateGradientAlgorithm(surface, surface, { bestUL, bestVL, bestUR, bestVR }, wrap);
}

glm::vec4 subdivisionInitialValue(const Parametric* surface1, const Parametric* surface2, const glm::vec4& wrap)
{
	static constexpr int subdivisions = 24;
	static constexpr float step = 1.0f / (subdivisions - 1);

	float dist = std::numeric_limits<float>::max();
	glm::vec4 X;
	
	for (float lu = 0.0f; lu <= 1.0f; lu += step)
	{
		for (float lv = 0.0f; lv <= 1.0f; lv += step)
		{
			for (float ru = 0.0f; ru <= 1.0f; ru += step)
			{
				for (float rv = 0.0f; rv <= 1.0f; rv += step)
				{
					auto P1 = surface1->evaluate(lu, lv);
					auto P2 = surface2->evaluate(ru, rv);
					float newDist = glm::length(P1 - P2);
					if (newDist < dist)
					{
						dist = newDist;
						X = { lu, lv, ru, rv };
					}
				}
			}
		}
	}
	return conjugateGradientAlgorithm(surface1, surface2, X, wrap);
}

glm::vec4 subdivisionSelf(const Parametric* surface, const glm::vec4& wrap)
{
	static constexpr int subdivisions = 40;
	static constexpr float step = 1.0f / (subdivisions - 1);

	float dist = std::numeric_limits<float>::max();
	glm::vec4 X;

	for (float lu = 0.0f; lu <= 1.0f; lu += step)
	{
		for (float lv = 0.0f; lv <= 1.0f; lv += step)
		{
			for (float ru = 0.0f; ru <= 1.0f; ru += step)
			{
				for (float rv = 0.0f; rv <= 1.0f; rv += step)
				{
					auto P1 = surface->evaluate(lu, lv);
					auto P2 = surface->evaluate(ru, rv);
					float newDist = glm::length(P1 - P2);
					if (newDist < dist && (
						(!wrap.x && std::abs(lu - ru) > 2e-1) ||
						(!wrap.y && std::abs(lv - rv) > 2e-1)))
					{
						dist = newDist;
						X = { lu, lv, ru, rv };
					}
				}
			}
		}
	}

	return conjugateGradientAlgorithm(surface, surface, X, wrap);
}

std::tuple<std::vector<glm::vec3>, std::vector<glm::vec4>> intersectionNewton(const Parametric* surface1, const Parametric* surface2, float d, const glm::vec4& wrap, const glm::vec4& initialValue)
{
	static constexpr int maxPoints = 200;
	static constexpr int maxIters = 1000;
	
	const float initialD = d;
	glm::vec4 X = initialValue;
	std::vector<glm::vec3> positions1{ surface1->evaluate(X.x, X.y) };
	std::vector<glm::vec3> positions2{ surface1->evaluate(X.x, X.y) };
	std::vector<glm::vec4> params1{ X };
	std::vector<glm::vec4> params2{ X };

	auto findPointsInDirection = [&](std::vector<glm::vec3>& positions, std::vector<glm::vec4>& params, float sgn)
	{
		int i = 0;
		int j;
		while (i < maxPoints)
		{
			float diff = std::numeric_limits<float>::max();
			j = 0;
			while (diff > 1e-4)
			{
				float u = X.x;
				float v = X.y;
				float s = X.z;
				float t = X.w;
				auto PU = surface1->derivativeU(u, v);
				auto PV = surface1->derivativeV(u, v);
				auto np = glm::cross(PU, PV);

				auto QU = surface2->derivativeU(s, t);
				auto QV = surface2->derivativeV(s, t);
				auto nq = glm::cross(QU, QV);

				if (glm::length(np - nq) <= math::eps)
					return;

				auto tangent = glm::normalize(glm::cross(np, nq)) * sgn;

				if (wrap.x && wrap.z && positions.size() >= 2 && glm::dot(tangent, positions[i] - positions[i - 1]) < 0)
				{
					tangent *= -1;
				}

				// Newtod iteration
				auto P = surface1->evaluate(u, v);
				auto Q = surface2->evaluate(s, t);
				auto fx = glm::vec4(P - Q, glm::dot(P - positions[i], tangent) - d);

				glm::mat4 Dfx{
					glm::vec4(PU, glm::dot(PU, tangent)),
					glm::vec4(PV, glm::dot(PV, tangent)),
					glm::vec4(-QU, 0),
					glm::vec4(-QV, 0)
				};

				auto inv = glm::inverse(Dfx);
				auto decrement = (inv * fx);
				X = X -  decrement / surface1->getRange(); // ???

				if (isnan(X))
					return;

				cap(X, wrap);
				auto newApprox1 = surface1->evaluate(X.x, X.y);
				auto newApprox2 = surface2->evaluate(X.z, X.w);
				if (isnan(newApprox1))
					return;

				auto newDiff = glm::length(newApprox1 - newApprox2);
				diff = newDiff;

				if (j++ > maxIters)
					return;
			}
			// Check boundaries
			auto newPos = surface1->evaluate(X.x, X.y);
			if (positions.size() > 1 && glm::length(positions[0] - newPos) < 1e-3)
			{
				positions.push_back(positions[0]);
				params.push_back(params[0]);
				return;
			}

			positions.push_back(newPos);
			params.push_back(X);
			i++;
		}
		endCurve(surface1, surface2, positions, params, d, wrap);
	};
	findPointsInDirection(positions1, params1, 1.0f);
	X = initialValue;
	d = initialD;
	findPointsInDirection(positions2, params2, -1.0f);

	std::vector<glm::vec3> finalPositions(positions1.rbegin(), positions1.rend() - 1);
	std::copy(positions2.begin(), positions2.end(), std::back_inserter(finalPositions));
	std::vector<glm::vec4> finalParams(params1.rbegin(), params1.rend() - 1);
	std::copy(params2.begin(), params2.end(), std::back_inserter(finalParams));

	return { finalPositions, finalParams };
}

export namespace math
{
	std::unique_ptr<IntersectionCurve> calculateIntersection(Parametric* surface1, Parametric* surface2, const Shader* shader, float d, const glm::vec4& wrap)
	{
		// Starting point not specified - find using subdivision
		glm::vec4 initialValue = subdivisionInitialValue(surface1, surface2, wrap);
		auto result = intersectionNewton(surface1, surface2, d, wrap, initialValue);
		return std::make_unique<IntersectionCurve>(surface1, surface2, std::get<0>(result), std::get<1>(result), shader);
	}

	std::unique_ptr<IntersectionCurve> calculateIntersection(Parametric* surface1, Parametric* surface2, const Shader* shader, float d, const glm::vec4& wrap, const glm::vec3& startingPointLocation)
	{
		// Starting point specified - find using conjugate gradient method
		glm::vec4 initialValue = conjugateGradientInitialValue(surface1, surface2, startingPointLocation, wrap);
		auto result = intersectionNewton(surface1, surface2, d, wrap, initialValue);
		return std::make_unique<IntersectionCurve>(surface1, surface2, std::get<0>(result), std::get<1>(result), shader);
	}

	std::unique_ptr<IntersectionCurve> calculateSelfIntersection(Parametric* surface, const Shader* shader, float d, const glm::vec4& wrap)
	{
		// Self intersection
		glm::vec4 initialValue = subdivisionSelf(surface, wrap);
		auto result = intersectionNewton(surface, surface, d, wrap, initialValue);
		return std::make_unique<IntersectionCurve>(surface, surface, std::get<0>(result), std::get<1>(result), shader);
	}

	std::unique_ptr<IntersectionCurve> calculateSelfIntersection(Parametric* surface, const Shader* shader, float d, const glm::vec4& wrap, const glm::vec3& startingPointLocation)
	{
		// Self intersection
		glm::vec4 initialValue = conjugateGradientSelf(surface, startingPointLocation, wrap);
		auto result = intersectionNewton(surface, surface, d, wrap, initialValue);
		return std::make_unique<IntersectionCurve>(surface, surface, std::get<0>(result), std::get<1>(result), shader);
	}
}