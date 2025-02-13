export module math:bezier;

import std;
import glm;

export namespace math
{
	inline constexpr float derivativeH = 1e-3f;

	auto deCasteljauSplit3(const glm::vec3& b00, const glm::vec3& b10, const glm::vec3& b20, const glm::vec3& b30, float t)
	{
		float u = (1.0f - t);

		auto b01 = b00 * u + b10 * t;
		auto b11 = b10 * u + b20 * t;
		auto b21 = b20 * u + b30 * t;

		auto b02 = b01 * u + b11 * t;
		auto b12 = b11 * u + b21 * t;

		auto b03 = b02 * u + b12 * t;

		std::array<glm::vec3, 7> ret;
		ret[0] = b00;
		ret[1] = b01;
		ret[2] = b02;
		ret[3] = b03;
		ret[4] = b12;
		ret[5] = b21;
		ret[6] = b30;

		return ret;
	}

	auto deCasteljau2(const auto& b00, const auto& b10, const auto& b20, float t)
	{
		float u = (1.0f - t);

		auto b01 = b00 * u + b10 * t;
		auto b11 = b10 * u + b20 * t;

		return b01 * u + b11 * t;
	}

	auto deCasteljau3(const auto& b00, const auto& b10, const auto& b20, const auto& b30, float t)
	{
		float u = (1.0f - t);

		auto b01 = b00 * u + b10 * t;
		auto b11 = b10 * u + b20 * t;
		auto b21 = b20 * u + b30 * t;

		auto b02 = b01 * u + b11 * t;
		auto b12 = b11 * u + b21 * t;

		return b02 * u + b12 * t;
	}

	auto deCasteljau3Derivative(const auto& b00, const auto& b10, const auto& b20, const auto& b30, float t)
	{
		float u = (1.0f - t);

		auto b01 = b00 * u + b10 * t;
		auto b11 = b10 * u + b20 * t;
		auto b21 = b20 * u + b30 * t;

		auto b02 = b01 * u + b11 * t;
		auto b12 = b11 * u + b21 * t;

		return 3.0f * (b12 - b02);
	}

	auto deBoorBasisFunctions(int i, float t, int knotCount)
	{
		float knotDist = 1.0f / knotCount;

		std::array<float, 5> N;
		std::array<float, 4> A;
		std::array<float, 4> B;

		N[1] = 1;
		for (int j = 1; j <= 3; j++)
		{
			A[j] = knotDist * (i + j) - t;
			B[j] = t - knotDist * (i + 1 - j);
			float saved = 0;
			for (int k = 1; k <= j; k++)
			{
				float term = N[k] / (A[k] + B[j + 1 - k]);
				N[k] = saved + A[k] * term;
				saved = B[j + 1 - k] * term;
			}
			N[j + 1] = saved;
		}

		return N;
	}
}