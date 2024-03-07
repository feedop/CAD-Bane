export module camera;

import <iostream>;

import <glm/vec3.hpp>;
import <glm/mat4x4.hpp>;
import <glm/gtc/constants.hpp>;


import math;

export class Camera
{
public:
	Camera(const glm::vec3& initialPosition) : position(initialPosition)
	{
		setAspect(1.0f);
		update();
	}

	Camera(float x, float y, float z) : Camera(glm::vec3{ x, y, z })
	{}

	inline const glm::vec3& getPosition() const
	{
		return position;
	}

	inline const glm::mat4& getView() const
	{
		return view;
	}

	inline const glm::mat4& getProjection() const
	{
		return projection;
	}

	inline glm::vec3 getZoomScale() const
	{
		return { 1 / zoomScale, 1 / zoomScale, 1 / zoomScale };
	}

	inline void setAspect(float aspect)
	{
		projection = math::perspective(math::pi / 4, aspect, 0.0001f, 1000.0f);
	}

	void rotate(float xAngleRadians, float yAngleRadians)
	{
		xRotation += up.y > 0 ? xAngleRadians : -xAngleRadians;
		if (xRotation > math::pi)
			xRotation = -math::pi;
		if (xRotation < -math::pi)
			xRotation = math::pi;

		yRotation += yAngleRadians;
		if (yRotation >= math::pi / 2 || yRotation <= -math::pi / 2)
			up.y = -1.0f;
		else
			up.y = 1.0f;

		if (yRotation >= math::pi)
			yRotation -= math::pi;
		else if (yRotation <= -math::pi)
			yRotation += math::pi;

		update();
	}

	void move(float xDiff, float yDiff)
	{
		center += glm::vec3(math::rotate(xRotation, { 0.0f, -1.0f, 0.0f })
			* math::rotate(yRotation, { -1.0f, 0.0f, 0.0f })
			* glm::vec4(-xDiff, yDiff, 0.0f, 1.0f));
		update();
	}

	void zoom(float offset)
	{
		static constexpr float coefficient = 0.2f;
		zoomScale += (offset > 0 ? -coefficient : coefficient);
		if (zoomScale < math::eps)
			zoomScale = math::eps;
		update();
	}

private:
	glm::vec3 position;
	glm::vec3 center{ 0.0f, 0.0f, 0.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };
	glm::mat4 view{ 1.0f };
	glm::mat4 projection = math::perspective(math::pi / 4, 1, 0.0001f, 1000.0f);
	float xRotation = 0;
	float yRotation = 0;
	float zoomScale = 1.0f;

	void update()
	{
		position =
			math::translate(center)
			* math::rotate(xRotation, { 0.0f, -1.0f, 0.0f })
			* math::rotate(yRotation, {-1.0f, 0.0f, 0.0f})
			* math::scale({ 0.0f, 0.0f, zoomScale })
			* glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		view = math::lookAt(position, center, up);
	}
};