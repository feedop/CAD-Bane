export module camera;

import <iostream>;

import <glm/vec3.hpp>;
import <glm/mat4x4.hpp>;
import <glm/gtc/constants.hpp>;


import math;

export class Camera
{
public:
	Camera(const glm::vec3& initialPosition) : translation(initialPosition)
	{
		setAspect(1.0f);
		update();
	}

	Camera(float x, float y, float z) : Camera(glm::vec3{ x, y, z })
	{}

	inline const glm::vec3& getPosition() const
	{
		return translation;
	}

	inline const glm::vec3& getRight() const
	{
		return right;
	}

	inline const glm::vec3& getUp() const
	{
		return up;
	}

	inline const glm::vec3& getFront() const
	{
		return front;
	}

	inline const glm::mat4& getView() const
	{
		return view;
	}

	inline const glm::mat4& getProjection() const
	{
		return projection;
	}

	inline const glm::vec3& getZoomScale() const
	{
		return { 1 / zoomScale, 1 / zoomScale, 1 / zoomScale };
	}

	inline void setAspect(float aspect)
	{
		projection = math::perspective(math::pi / 4, aspect, 0.02f, 200.0f);
	}

	void rotate(float xAngleRadians, float yAngleRadians)
	{
		xRotation += up.y > 0 ? xAngleRadians : -xAngleRadians;
		if (xRotation > math::pi)
			xRotation = -math::pi;
		if (xRotation < -math::pi)
			xRotation = math::pi;

		yRotation += yAngleRadians;
		if (yRotation > math::pi)
			yRotation - math::pi;
		else if (yRotation < -math::pi)
			yRotation = math::pi;

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
	inline static const glm::vec3 initialRight{ 1.0f, 0.0f, 0.0f };
	inline static const glm::vec3 initialUp{ 0.0f, 1.0f, 0.0f };
	inline static const glm::vec3 initialFront{ 0.0f, 0.0f, -1.0f };

	glm::vec3 translation;
	glm::vec3 center{ 0.0f, 0.0f, 0.0f };
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 front;

	glm::mat4 view{ 1.0f };
	glm::mat4 projection = math::perspective(math::pi / 4, 1, 0.0001f, 1000.0f);
	float xRotation = 0;
	float yRotation = math::pi / 6;
	float zoomScale = 2.0f;

	void update()
	{
		auto rotation = math::rotate(xRotation, { 0.0f, -1.0f, 0.0f })
			* math::rotate(yRotation, { -1.0f, 0.0f, 0.0f });

		translation = 
			math::translate(center)
			* rotation
			* math::scale({ 0.0f, 0.0f, zoomScale })
			* glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

		right = rotation * glm::vec4(initialRight, 1.0f);
		up = rotation * glm::vec4(initialUp, 1.0f);
		front = rotation * glm::vec4(initialFront, 1.0f);

		view = math::lookAt(translation, center, up);

	}
};