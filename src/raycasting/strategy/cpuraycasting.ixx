export module cpuraycasting;

import <glad/glad.h>;
import <algorithm>;
import <execution>;
import <ranges>;
import <vector>;

import <glm/vec4.hpp>;
import <glm/mat4x4.hpp>;
import <glm/ext/matrix_transform.hpp>;
import <glm/gtx/matrix_operation.hpp>;

import camera;
import ellipsoid;
import math;
import raycastingstrategy;

export class CpuRaycasting : public RaycastingStrategy
{
public:
	CpuRaycasting(const Camera& camera, const Ellipsoid& ellipsoid) : RaycastingStrategy(camera, ellipsoid)
	{
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	virtual void update(int width, int height, int lightM) override
	{
		if (texture.size() < width * height)
		{
			texture.resize(width * height);
		}

		auto range = std::ranges::views::iota(0, width * height);
		std::for_each(std::execution::par, range.begin(), range.end(), [&](int pixel) { perPixel(pixel, width, height, lightM); });

		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, texture.data());
	}

private:
	std::vector<glm::vec4> texture;

	/// <summary>
	/// Executed for each texture pixel, preferably in parallel
	/// </summary>
	/// <param name="i">Pixel index</param>
	void perPixel(int i, int width, int height, int lightM)
	{
		int screenX = i / width;
		int screenY = i % height;

		float x = xScreen2View(screenX, width);
		float y = yScreen2View(screenY, height);

		// Raycasting
		glm::mat4 D = glm::diagonal4x4(glm::vec4{
			1 / (ellipsoid.a * ellipsoid.a),
			1 / (ellipsoid.b * ellipsoid.b),
			1 / (ellipsoid.c * ellipsoid.c),
			-1
		});

		glm::mat4 M = camera.getView() * math::scale(camera.getZoomScale());
		glm::mat4 Minv = glm::inverse(M);
		glm::mat4 Dm = glm::transpose(Minv) * D * Minv;

		float a = Dm[2][2];
		float b = Dm[3][2] + Dm[2][3] + x * (Dm[0][2] + Dm[2][0]) + y * (Dm[1][2] + Dm[2][1]);
		float c = Dm[3][3] + x * (Dm[3][0] + Dm[0][3] + x * Dm[0][0] + y * Dm[0][1]) + y * (Dm[3][1] + Dm[1][3] + x * Dm[1][0] + y * Dm[1][1]);

		// Check quadratic equation solutions
		float delta = b * b - 4 * a * c;
		if (delta > 0)
		{
			float deltaRoot = std::sqrt(delta);
			float z1 = (-b + deltaRoot) / (2 * a);
			float z2 = (-b - deltaRoot) / (2 * a);
			float z = closer(x, y, z1, z2) ? z1 : z2;
			glm::vec3 pixelPosition{ x, y, z };

			texture[i] = { Ellipsoid::color * (math::specular(Dm, pixelPosition, lightM)), 1.0f };
		}
		else
			texture[i] = { 0.0f, 0.0f, 0.0f, 0.0f };
	}

	/// <summary>
	/// Whether z1 is closer to the camera than z2 with set x and y
	/// </summary>
	inline bool closer(float x, float y, float z1, float z2) const
	{
		if (z1 == z2)
			return true;

		const glm::vec3& cameraPos = camera.getPosition();
		return glm::length(glm::vec3{ x , y, z1 }) < glm::length(glm::vec3{ x, y, z2 });
	}

	inline float xScreen2View(int pixel, int width) const
	{
		return static_cast<float>(pixel) / width * 2 - 1.0f;
	}

	inline float yScreen2View(int pixel, int height) const
	{
		return -(static_cast<float>(pixel) / height * 2 - 1.0f);
	}
};