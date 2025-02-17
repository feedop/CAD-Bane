export module cpuraycasting;

import std;
import glm;

import <glad/glad.h>;

import camera;
import ellipsoid;
import math;
import raycastingstrategy;

/// <summary>
/// Class that implements the raycasting strategy using the CPU for rendering the ellipsoid to a texture.
/// </summary>
export class CpuRaycasting : public RaycastingStrategy
{
public:
	/// <summary>
	/// Constructor that initializes the CPU raycasting strategy with the given camera and ellipsoid.
	/// It also sets up the texture for raycasting.
	/// </summary>
	/// <param name="camera">The camera used for rendering the raycast.</param>
	/// <param name="ellipsoid">The ellipsoid geometry to be used in raycasting.</param>
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

	/// <summary>
	/// Updates the raycasting texture based on the width, height, and shininess.
	/// This method will parallelize the computation of each pixel.
	/// </summary>
	/// <param name="width">The width of the texture.</param>
	/// <param name="height">The height of the texture.</param>
	/// <param name="lightM">Phoneg shininess.</param>
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
	/// This method is executed for each pixel in the texture, calculating the color for that pixel
	/// based on the raycasting technique.
	/// </summary>
	/// <param name="i">The index of the pixel being processed.</param>
	/// <param name="width">The width of the texture.</param>
	/// <param name="height">The height of the texture.</param>
	/// <param name="lightM">Phong shininess.</param>
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
	/// Determines whether z1 is closer to the camera than z2, based on the set x and y coordinates.
	/// </summary>
	/// <param name="x">The x-coordinate of the pixel.</param>
	/// <param name="y">The y-coordinate of the pixel.</param>
	/// <param name="z1">The first z-value.</param>
	/// <param name="z2">The second z-value.</param>
	/// <returns>True if z1 is closer to the camera than z2, otherwise false.</returns>
	inline bool closer(float x, float y, float z1, float z2) const
	{
		if (z1 == z2)
			return true;

		const glm::vec3& cameraPos = camera.getPosition();
		return glm::length(glm::vec3{ x , y, z1 }) < glm::length(glm::vec3{ x, y, z2 });
	}

	/// <summary>
	/// Converts the screen-space x-coordinate to the normalized device coordinate (view space).
	/// </summary>
	/// <param name="pixel">The screen-space x-coordinate of the pixel.</param>
	/// <param name="width">The width of the screen or viewport.</param>
	/// <returns>The x-coordinate in normalized device coordinates (-1 to 1).</returns>
	inline float xScreen2View(int pixel, int width) const
	{
		return static_cast<float>(pixel) / width * 2 - 1.0f;
	}

	/// <summary>
	/// Converts the screen-space y-coordinate to the normalized device coordinate (view space).
	/// </summary>
	/// <param name="pixel">The screen-space y-coordinate of the pixel.</param>
	/// <param name="height">The height of the screen or viewport.</param>
	/// <returns>The y-coordinate in normalized device coordinates (-1 to 1).</returns>
	inline float yScreen2View(int pixel, int height) const
	{
		return -(static_cast<float>(pixel) / height * 2 - 1.0f);
	}
};