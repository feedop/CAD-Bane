export module intersections;


import std;
import glm;

import intersectioncurve;
import parametric;
import math;

/// <summary>
/// Checks if any component of the given glm::vec3 is NaN (Not a Number).
/// </summary>
/// <param name="X">The glm::vec3 to check for NaN components.</param>
/// <returns>
/// True if any component (x, y, or z) of the glm::vec3 is NaN, otherwise false.
/// </returns>
inline bool isnan(const glm::vec3& X)
{
	return glm::isnan(X.x) ||
		glm::isnan(X.y) ||
		glm::isnan(X.z);
}

/// <summary>
/// Checks if any component of the given glm::vec4 is NaN (Not a Number).
/// </summary>
/// <param name="X">The glm::vec4 to check for NaN components.</param>
/// <returns>
/// True if any component (x, y, z, or w) of the glm::vec4 is NaN, otherwise false.
/// </returns>
inline bool isnan(const glm::vec4& X)
{
	return glm::isnan(X.x) ||
		glm::isnan(X.y) ||
		glm::isnan(X.z) ||
		glm::isnan(X.w);
}

/// <summary>
/// Compares two values for equality, considering a small epsilon tolerance.
/// </summary>
/// <param name="v1">The first value to compare.</param>
/// <param name="v2">The second value to compare.</param>
/// <returns>
/// True if the absolute difference between the values is less than or equal to a small epsilon, indicating equality.
/// </returns>
inline bool eq(auto v1, auto v2)
{
	return std::abs(v1 - v2) <= math::eps;
}

/// <summary>
/// Closes the intersection curve if the surface wraps in the desired direction and the curve's start and end point are sufficiently close
/// </summary>
/// <param name="surface1">A pointer to the first parametric surface.</param>
/// <param name="surface2">A pointer to the second parametric surface.</param>
/// <param name="positions">A vector of curve positions in 3D space.</param>
/// <param name="params">A vector representing the curve in parameter space (on the first surface).</param>
/// <param name="d">The desired distance between curve points.</param>
/// <param name="wrap">A glm::vec4 specifying the wrapping limits for both surfaces in each direction.</param>
/// <param name="toolRadius">The surfaces' offset along their normal vector.</param>
void endCurve(const Parametric* surface1, const Parametric* surface2, std::vector<glm::vec3>& positions, std::vector<glm::vec4>& params, float d, const glm::vec4& wrap, float toolRadius)
{
	d *= 4;
	auto& X = params.back();
	auto& pos = positions.back();
	if (!wrap.x)
	{
		if (glm::length(pos - surface1->evaluate(0.0f, X.y, toolRadius)) <= d)
		{
			glm::vec4 newX{ 0.0f, X.y, X.z, X.w };
			params.push_back(newX);
			positions.push_back(surface1->evaluate(newX.x, newX.y, toolRadius));
		}
		if (glm::length(pos - surface1->evaluate(1.0f, X.y, toolRadius)) <= d)
		{
			glm::vec4 newX{ 1.0f, X.y, X.z, X.w };
			params.push_back(newX);
			positions.push_back(surface1->evaluate(newX.x, newX.y, toolRadius));
		}
	}
	else if (!wrap.y)
	{
		if (glm::length(pos - surface1->evaluate(X.x, 0.0f, toolRadius)) <= d)
		{
			glm::vec4 newX{ X.x, 0.0f, X.z, X.w };
			params.push_back(newX);
			positions.push_back(surface1->evaluate(newX.x, newX.y, toolRadius));
		}
		if (glm::length(pos - surface1->evaluate(X.x, 1.0f, toolRadius)) <= d)
		{
			glm::vec4 newX{ X.x, 1.0f, X.z, X.w };
			params.push_back(newX);
			positions.push_back(surface1->evaluate(newX.x, newX.y, toolRadius));
		}
	}
	else if (!wrap.z)
	{
		if (glm::length(pos - surface2->evaluate(0.0f, X.w, toolRadius)) <= d)
		{
			glm::vec4 newX{ X.x, X.y, 0.0f, X.w };
			params.push_back(newX);
			positions.push_back(surface2->evaluate(newX.x, newX.y, toolRadius));
		}
		if (glm::length(pos - surface2->evaluate(1.0f, X.w, toolRadius)) <= d)
		{
			glm::vec4 newX{ X.x, X.y, 1.0f, X.w };
			params.push_back(newX);
			positions.push_back(surface2->evaluate(newX.x, newX.y, toolRadius));
		}
	}
	else if (!wrap.w)
	{
		if (glm::length(pos - surface2->evaluate(X.z, 0.0f, toolRadius)) <= d)
		{
			glm::vec4 newX{ X.x, X.y, X.z, 0.0f };
			params.push_back(newX);
			positions.push_back(surface2->evaluate(newX.x, newX.y, toolRadius));
		}
		if (glm::length(pos - surface2->evaluate(X.z, 1.0f, toolRadius)) <= d)
		{
			glm::vec4 newX{ X.x, X.y, X.z, 1.0f };
			params.push_back(newX);
			positions.push_back(surface2->evaluate(newX.x, newX.y, toolRadius));
		}
	}
}

/// <summary>
/// Caps or wraps the value within the specified limits.
/// </summary>
/// <param name="value">The value to cap or wrap.</param>
/// <param name="wrap">If true, wraps the value around the limits. If false, caps the value at the limits.</param>
/// <param name="lowerLimit">The lower limit for the value (default is 0.0f).</param>
/// <param name="upperLimit">The upper limit for the value (default is 1.0f).</param>
void cap(float& value, bool wrap, float lowerLimit = 0.0f, float upperLimit = 1.0f)
{
	if (wrap)
	{
		while (value < lowerLimit)
			value = upperLimit;
		while (value > upperLimit)
			value = lowerLimit;
	}
	else
	{
		if (value < lowerLimit)
			value = lowerLimit;
		else if (value > upperLimit)
			value = upperLimit;
	}
}

/// <summary>
/// Caps the components of a glm::vec4 vector individually using a specified wrap vector.
/// </summary>
/// <param name="vec">The glm::vec4 vector whose components will be capped.</param>
/// <param name="wrap">A glm::vec4 specifying the wrap limits for each component of the vector.</param>
void cap(glm::vec4& vec, const glm::vec4& wrap)
{
	cap(vec.x, wrap.x);
	cap(vec.y, wrap.y);
	cap(vec.z, wrap.z);
	cap(vec.w, wrap.w);
}

/// <summary>
/// Calculates the gradient between two parametric surfaces at a specific point.
/// </summary>
/// <param name="X">A glm::vec4 representing the coordinates at which the gradient is calculated. The first two components are used for surface1, and the last two for surface2.</param>
/// <param name="surface1">A pointer to the first parametric surface.</param>
/// <param name="surface2">A pointer to the second parametric surface.</param>
/// <param name="toolRadius">The surfaces' offset along their normal vector.</param>
/// <returns>
/// A glm::vec4 representing the gradient calculated from the differences in surface evaluations and their derivatives.
/// </returns>
glm::vec4 calculateGradient(const glm::vec4 X, const Parametric* surface1, const Parametric* surface2, float toolRadius)
{
	auto ev1 = surface1->evaluate(X.x, X.y, toolRadius);
	auto ev2 = surface2->evaluate(X.z, X.w, toolRadius);
	auto diff = ev1 - ev2;

	auto du1 = surface1->derivativeU(X.x, X.y, toolRadius);
	auto dv1 = surface1->derivativeV(X.x, X.y, toolRadius);

	auto du2 = surface2->derivativeU(X.z, X.w, toolRadius);
	auto dv2 = surface2->derivativeV(X.z, X.w, toolRadius);

	return 2.0f * glm::vec4{
		glm::dot(diff, du1),
		glm::dot(diff, dv1),
		glm::dot(-diff, du2),
		glm::dot(-diff, dv2)
	};
}

/// <summary>
/// Performs the conjugate gradient algorithm to find the point where the difference between two parametric surfaces is minimized.
/// </summary>
/// <param name="surface1">A pointer to the first parametric surface.</param>
/// <param name="surface2">A pointer to the second parametric surface.</param>
/// <param name="X">A glm::vec4 representing the initial guess for the solution.</param>
/// <param name="wrap">A glm::vec4 specifying the wrapping limits for both surfaces in each direction.</param>
/// <param name="toolRadius">The surfaces' offset along their normal vector.</param>
/// <returns>
/// A glm::vec4 representing the solution, where the difference between the two parametric surfaces is minimized.
/// </returns>
glm::vec4 conjugateGradientAlgorithm(const Parametric* surface1, const Parametric* surface2, glm::vec4 X, const glm::vec4& wrap, float toolRadius = 0.0f)
{
	static constexpr int maxIters = 2500;
	float cgStep = 1e-4;

	float dist = std::numeric_limits<float>::max();

	int i = 0;
	while (dist > math::eps && i < maxIters && cgStep > 1e-6)
	{
		auto gradient = calculateGradient(X, surface1, surface2, toolRadius);
		X -= gradient * cgStep;
		cap(X, wrap);
		float newDist = glm::length(surface1->evaluate(X.x, X.y, toolRadius) - surface2->evaluate(X.z, X.w, toolRadius));
		if (newDist >= dist)
			cgStep /= 2;

		dist = newDist;
		i++;
	}

	return X;
}

/// <summary>
/// Calculates the initial values for the intersection algorithm using conjugate gradients.
/// </summary>
/// <param name="surface1">A pointer to the first parametric surface.</param>
/// <param name="surface2">A pointer to the second parametric surface.</param>
/// <param name="startingPointLocation">A glm::vec3 representing the location of the starting point to match on both surfaces.</param>
/// <param name="wrap">A glm::vec4 specifying the wrapping limits for both surfaces in each direction.</param>
/// <param name="toolRadius">The surfaces' offset along their normal vector.</param>
/// <returns>
/// A glm::vec4 representing the initial values from the conjugate gradient algorithm to minimize the distance between the two surfaces.
/// </returns>
glm::vec4 conjugateGradientInitialValue(const Parametric* surface1, const Parametric* surface2, const glm::vec3& startingPointLocation, const glm::vec4& wrap, float toolRadius = 0.0f)
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
			float newDist = glm::length(startingPointLocation - surface1->evaluate(u, v, toolRadius));
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
			float newDist = glm::length(startingPointLocation - surface2->evaluate(u, v, toolRadius));
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

/// <summary>
/// Calculates the initial values for a self-intersection using conjugate gradients.
/// </summary>
/// <param name="surface1">A pointer to the parametric surface.</param>
/// <param name="startingPointLocation">A glm::vec3 representing the location of the starting point to match on both surfaces.</param>
/// <param name="wrap">A glm::vec4 specifying the wrapping limits for both surfaces in each direction.</param>
/// <param name="toolRadius">The surface's offset along its normal vector.</param>
/// <returns>
/// A glm::vec4 representing the initial values from the conjugate gradient algorithm to minimize the distance between two distant points on the surface.
/// </returns>
glm::vec4 conjugateGradientSelf(const Parametric* surface, const glm::vec3& startingPointLocation, const glm::vec4& wrap, float toolRadius = 0.0f)
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
			float newDist = glm::length(startingPointLocation - surface->evaluate(u, v, toolRadius));
			if (newDist < dist)
			{
				dist = newDist;
				bestUL = u;
				bestVL = v;
			}
		}
	}

	auto closestPoint = surface->evaluate(bestUL, bestVL, toolRadius);

	// Find a point that's furthest from previously found
	float bestUR = 0;
	float bestVR = 0;
	dist = std::numeric_limits<float>::max();
	for (float u = 0; u <= 1; u += step)
	{
		for (float v = 0; v <= 1; v += step)
		{
			auto point = surface->evaluate(u, v, toolRadius);
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

/// <summary>
/// Calculates the initial values for the intersection algorithm using simple subdivision.
/// </summary>
/// <param name="surface1">A pointer to the first parametric surface.</param>
/// <param name="surface2">A pointer to the second parametric surface.</param>
/// <param name="startingPointLocation">A glm::vec3 representing the location of the starting point to match on both surfaces.</param>
/// <param name="wrap">A glm::vec4 specifying the wrapping limits for both surfaces in each direction.</param>
/// <param name="toolRadius">The surfaces' offset along their normal vector.</param>
/// <returns>
/// A glm::vec4 representing the initial values from the subdivision algorithm to minimize the distance between the two surfaces.
/// </returns>
glm::vec4 subdivisionInitialValue(const Parametric* surface1, const Parametric* surface2, const glm::vec4& wrap, float toolRadius = 0.0f)
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
					auto P1 = surface1->evaluate(lu, lv, toolRadius);
					auto P2 = surface2->evaluate(ru, rv, toolRadius);
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

/// <summary>
/// Calculates the initial values for a self-intersection using simple subdivision.
/// </summary>
/// <param name="surface1">A pointer to the parametric surface.</param>
/// <param name="startingPointLocation">A glm::vec3 representing the location of the starting point to match on both surfaces.</param>
/// <param name="wrap">A glm::vec4 specifying the wrapping limits for both surfaces in each direction.</param>
/// <param name="toolRadius">The surface's offset along its normal vector.</param>
/// <returns>
/// A glm::vec4 representing the initial values from the subdivision to minimize the distance between two distant points on the surface.
/// </returns>
glm::vec4 subdivisionSelf(const Parametric* surface, const glm::vec4& wrap, float toolRadius = 0.0f)
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
					auto P1 = surface->evaluate(lu, lv, toolRadius);
					auto P2 = surface->evaluate(ru, rv, toolRadius);
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

/// <summary>
/// Default point adder. Use a custom function for debug purposes.
/// </summary>
/// <param name="">The point to add</param>
void defaultAddPoint(const glm::vec3&){}

/// <summary>
/// Finds the intersection points between two parametric surfaces using the Newton-Raphson method.
/// </summary>
/// <param name="surface1">A pointer to the first parametric surface.</param>
/// <param name="surface2">A pointer to the second parametric surface.</param>
/// <param name="d">A float representing the distance threshold to consider for the intersection.</param>
/// <param name="wrap">A glm::vec4 specifying the wrapping limits for both surfaces in each direction.</param>
/// <param name="initialValue">A glm::vec4 representing the initial guess for the solution parameters.</param>
/// <param name="toolRadius1">The first surface's offset along its normal vector. (default is 0.0f).</param>
/// <param name="addPoint">A function to add points to a collection, used for debug purposes (default is defaultAddPoint).</param>
/// <param name="toolRadius2">The second surface's offset along its normal vector.</param>
/// <returns>
/// A tuple containing two vectors:
/// 1. A vector of glm::vec3 representing the intersection points on surface1.
/// 2. A vector of glm::vec4 representing the solution parameters corresponding to the intersection points.
/// </returns>
std::tuple<std::vector<glm::vec3>, std::vector<glm::vec4>> intersectionNewton(
	const Parametric* surface1, const Parametric* surface2, float d, const glm::vec4& wrap, const glm::vec4& initialValue,
	float toolRadius1 = 0.0f, std::function<void(const glm::vec3&)> addPoint = defaultAddPoint, float toolRadius2 = -1.0f)
{
	if (toolRadius2 < 0)
		toolRadius2 = toolRadius1;

	static constexpr int maxPoints = 1000;
	static constexpr int maxIters = 1000;
	const float initialD = d;
	glm::vec4 X = initialValue;
	std::vector<glm::vec3> positions1{ surface1->evaluate(X.x, X.y, toolRadius1) };
	std::vector<glm::vec3> positions2{ surface1->evaluate(X.x, X.y, toolRadius2) };

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
				auto PU = surface1->derivativeU(u, v, toolRadius1);
				auto PV = surface1->derivativeV(u, v, toolRadius1);
				auto np = glm::cross(PU, PV);

				auto QU = surface2->derivativeU(s, t, toolRadius2);
				auto QV = surface2->derivativeV(s, t, toolRadius2);
				auto nq = glm::cross(QU, QV);

				if (glm::length(np - nq) <= math::eps)
					return;

				auto tangent = glm::normalize(glm::cross(np, nq)) * sgn;

				if (wrap.x && wrap.z && positions.size() >= 2 && glm::dot(tangent, positions[i] - positions[i - 1]) < 0)
				{
					tangent *= -1;
				}

				// Newton iteration
				auto P = surface1->evaluate(u, v, toolRadius1);
				auto Q = surface2->evaluate(s, t, toolRadius2);

				auto fx = glm::vec4(P - Q, glm::dot(P - positions[i], tangent) - d);

				glm::mat4 Dfx{
					glm::vec4(PU, glm::dot(PU, tangent)),
					glm::vec4(PV, glm::dot(PV, tangent)),
					glm::vec4(-QU, 0),
					glm::vec4(-QV, 0)
				};

				auto inv = glm::inverse(Dfx);
				auto decrement = (inv * fx);
				X = X - decrement / surface1->getRange(); // ???

				if (isnan(X))
					return;

				cap(X, wrap);
				auto newApprox1 = surface1->evaluate(X.x, X.y, toolRadius1);
				auto newApprox2 = surface2->evaluate(X.z, X.w, toolRadius2);

				if (isnan(newApprox1))
					return;

				auto newDiff = glm::length(newApprox1 - newApprox2);
				diff = newDiff;

				if (j++ > maxIters)
					return;
			}
			// Check boundaries
			auto newPos = surface1->evaluate(X.x, X.y, toolRadius1);
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
		endCurve(surface1, surface2, positions, params, d, wrap, toolRadius1);
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

void debugSurface(Parametric* surface1, float toolRadius, auto addPoint)
{
	static constexpr float step = 0.01f;
	for (float u = 0.0f; u <= 1.0f + math::eps; u += step)
	{
		for (float v= 0.0f; v <= 1.0f + math::eps; v += step)
		{
			auto P = surface1->evaluate(u, v, toolRadius);
			addPoint(P);
		}
	}
}

// Various variants of intersection calculation functions
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

	auto calculateToolDistantIntersection(Parametric* surface1, Parametric* surface2, float d, const glm::vec4& wrap, float toolRadius, std::function<void(const glm::vec3&)> addPoint)
	{
		// Starting point not specified - find using subdivision
		glm::vec4 initialValue = subdivisionInitialValue(surface1, surface2, wrap);
		auto result = intersectionNewton(surface1, surface2, d, wrap, initialValue, toolRadius, addPoint);
		return result;
	}

	auto calculateToolDistantIntersection(Parametric* surface1, Parametric* surface2, float d, const glm::vec4& wrap, float toolRadius, std::function<void(const glm::vec3&)> addPoint, const glm::vec3& startingPointLocation)
	{
		// Starting point specified - find using conjugate gradient method
		glm::vec4 initialValue = conjugateGradientInitialValue(surface1, surface2, startingPointLocation, wrap);
		auto result = intersectionNewton(surface1, surface2, d, wrap, initialValue, toolRadius, addPoint);
		return result;
	}

	auto calculateToolDistantIntersection(Parametric* surface1, Parametric* surface2, float d, const glm::vec4& wrap, float toolRadius1, float toolRadius2, std::function<void(const glm::vec3&)> addPoint)
	{
		// Starting point not specified - find using subdivision
		glm::vec4 initialValue = subdivisionInitialValue(surface1, surface2, wrap);
		auto result = intersectionNewton(surface1, surface2, d, wrap, initialValue, toolRadius1, addPoint, toolRadius2);
		return result;
	}

	auto calculateToolDistantIntersection(Parametric* surface1, Parametric* surface2, float d, const glm::vec4& wrap, float toolRadius1, float toolRadius2, std::function<void(const glm::vec3&)> addPoint, const glm::vec3& startingPointLocation)
	{
		// Starting point specified - find using conjugate gradient method
		glm::vec4 initialValue = conjugateGradientInitialValue(surface1, surface2, startingPointLocation, wrap);
		auto result = intersectionNewton(surface1, surface2, d, wrap, initialValue, toolRadius1, addPoint, toolRadius2);
		return result;
	}
}