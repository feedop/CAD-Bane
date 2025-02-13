export module gpuraycasting;

import std;
import glm;

import <glad/glad.h>;

import canvas;
import math;
import raycastingstrategy;
import shader;

export class GpuRaycasting : public RaycastingStrategy
{
public:
	GpuRaycasting(const Camera& camera, const Ellipsoid& ellipsoid, const Canvas& canvas) : RaycastingStrategy(camera, ellipsoid), canvas(canvas)
	{
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, 0);
		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers); // "1" is the size of DrawBuffers
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void update(int width, int height, int lightM) override
	{
		if (texture.size() < 4 *width * height)
			texture.resize(4 * width * height);

		raycastingShader->use();
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data());
		glViewport(0, 0, width, height);

		raycastingShader->setInt("lightM", lightM);
		raycastingShader->setInt("width", width);
		raycastingShader->setInt("height", height);
		
		
		raycastingShader->setVector("ellipsoidColor", Ellipsoid::color);
		raycastingShader->setMatrix("Dm", calculateDm());
		canvas.draw(raycastingShader.get());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

private:
	std::vector<unsigned char> texture;
	unsigned int framebuffer = 0;

	std::unique_ptr<Shader> raycastingShader = std::make_unique<RaycastingShader>();

	const Canvas& canvas;

	glm::mat4 calculateDm()
	{
		glm::mat4 D = glm::diagonal4x4(glm::vec4{
			1 / (ellipsoid.a * ellipsoid.a),
			1 / (ellipsoid.b * ellipsoid.b),
			1 / (ellipsoid.c * ellipsoid.c),
			-1
		});

		glm::mat4 M = camera.getView() * math::scale(camera.getZoomScale());
		glm::mat4 Minv = glm::inverse(M);
		glm::mat4 Dm = glm::transpose(Minv) * D * Minv;
		return Dm;
	}
};