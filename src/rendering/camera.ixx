export module camera;

import std;
import glm;

import math;

/// <summary>
/// Represents a camera with position, rotation, and projection settings.
/// Supports movement, zoom, and stereo projection for left and right eyes.
/// </summary>
export class Camera
{
public:
	/// <summary>
	/// Constructs a Camera with an initial position.
	/// </summary>
	/// <param name="initialPosition">The initial position of the camera.</param>
	Camera(const glm::vec3& initialPosition) : translation(initialPosition)
	{
		setAspect(1.0f);
		calculateRedProjection();
		calculateBlueProjection();
		update();
	}

	/// <summary>
	/// Constructs a Camera at a specified position.
	/// </summary>
	/// <param name="x">The X coordinate.</param>
	/// <param name="y">The Y coordinate.</param>
	/// <param name="z">The Z coordinate.</param>
	Camera(float x, float y, float z) : Camera(glm::vec3{ x, y, z })
	{}

	/// <summary>
	/// Gets the camera position.
	/// </summary>
	/// <returns>The position as a glm::vec3.</returns>
	inline const glm::vec3& getPosition() const
	{
		return translation;
	}

	/// <summary>
	/// Gets the right vector of the camera.
	/// </summary>
	/// <returns>The right direction as a glm::vec3.</returns>
	inline const glm::vec3& getRight() const
	{
		return right;
	}

	/// <summary>
	/// Gets the up vector of the camera.
	/// </summary>
	/// <returns>The up direction as a glm::vec3.</returns>
	inline const glm::vec3& getUp() const
	{
		return up;
	}

	/// <summary>
	/// Gets the front vector of the camera.
	/// </summary>
	/// <returns>The front direction as a glm::vec3.</returns>
	inline const glm::vec3& getFront() const
	{
		return front;
	}

	// <summary>
	/// Gets the view matrix of the camera.
	/// </summary>
	/// <returns>The view matrix as a glm::mat4.</returns>
	inline const glm::mat4 getView() const
	{
		return view;
	}

	/// <summary>
	/// Gets the projection matrix of the camera.
	/// </summary>
	/// <returns>The projection matrix as a glm::mat4.</returns>
	inline const glm::mat4 getProjection() const
	{
		return projection;
	}

	/// <summary>
	/// Gets the zoom scale as a vector.
	/// </summary>
	/// <returns>A glm::vec3 representing the zoom scale.</returns>
	inline glm::vec3 getZoomScale() const
	{
		return { 1 / zoomScale, 1 / zoomScale, 1 / zoomScale };
	}

	/// <summary>
	/// Sets the aspect ratio of the camera projection.
	/// </summary>
	/// <param name="aspect">The new aspect ratio.</param>
	inline void setAspect(float aspect)
	{
		this->aspect = aspect;
		projection = math::perspective(math::pi / 4, aspect, 0.02f, 200.0f);
	}

	/// <summary>
	/// Rotates the camera by the given angles.
	/// </summary>
	/// <param name="xAngleRadians">Rotation around the X-axis (in radians).</param>
	/// <param name="yAngleRadians">Rotation around the Y-axis (in radians).</param>
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

	/// <summary>
	/// Moves the camera by the specified amount.
	/// </summary>
	/// <param name="xDiff">Movement in the X direction.</param>
	/// <param name="yDiff">Movement in the Y direction.</param>
	void move(float xDiff, float yDiff)
	{
		center += glm::vec3(math::rotate(xRotation, { 0.0f, -1.0f, 0.0f })
			* math::rotate(yRotation, { -1.0f, 0.0f, 0.0f })
			* glm::vec4(-xDiff, yDiff, 0.0f, 1.0f));
		update();
	}

	/// <summary>
	/// Zooms the camera in or out.
	/// </summary>
	/// <param name="offset">The zoom offset value.</param>
	void zoom(float offset)
	{
		static constexpr float coefficient = 0.2f;
		zoomScale += (offset > 0 ? -coefficient : coefficient);
		if (zoomScale < math::eps)
			zoomScale = math::eps;
		update();
	}

	// <summary>
	/// Gets the red eye projection matrix.
	/// </summary>
	/// <returns>The red projection matrix as a glm::mat4.</returns>
	inline const glm::mat4& getRedProjection() const
	{
		return redProjection;
	}

	/// <summary>
	/// Gets the blue eye projection matrix.
	/// </summary>
	/// <returns>The blue projection matrix as a glm::mat4.</returns>
	inline const glm::mat4& getBlueProjection() const
	{
		return blueProjection;
	}

	/// <summary>
	/// Calculates the projection for the red eye.
	/// </summary>
	inline void calculateRedProjection()
	{
		redProjection = frustum(leftEye, -1.0f);
	}

	/// <summary>
	/// Calculates the projection for the blue eye.
	/// </summary>
	inline void calculateBlueProjection()
	{
		blueProjection = frustum(rightEye, 1.0f);
	}

	/// <summary>
	/// Sets the camera for the left eye view.
	/// </summary>
	inline void setForLeftEye()
	{
		currentEye = -leftEye;
		update();
	}

	/// <summary>
	/// Sets the camera for the right eye view.
	/// </summary>
	inline void setForRightEye()
	{
		currentEye = rightEye;
		update();
	}

	/// <summary>
	/// Sets the camera for the center view.
	/// </summary>
	inline void setForCenter()
	{
		currentEye = 0.0f;
		update();
	}

private:
	friend class GuiController;

	inline static const glm::vec3 initialRight{ 1.0f, 0.0f, 0.0f };
	inline static const glm::vec3 initialUp{ 0.0f, 1.0f, 0.0f };
	inline static const glm::vec3 initialFront{ 0.0f, 0.0f, -1.0f };

	inline static constexpr float near = 0.0001f;
	inline static constexpr float far = 1000.0f;
	inline static constexpr float fov = math::pi / 4;

	glm::vec3 translation;
	glm::vec3 center{ 0.0f, 0.0f, 0.0f };
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 front;

	glm::mat4 view{ 1.0f };
	glm::mat4 projection = math::perspective(fov, 1, near, far);
	glm::mat4 redProjection = projection;
	glm::mat4 blueProjection = projection;

	float aspect = 1.0f;

	float xRotation = 0;
	float yRotation = math::pi / 6;
	float zoomScale = 20.0f;

	float leftEye = 0.003f, rightEye = 0.003f;
	float currentEye = 0.0f;
	float projectionPlaneDist = 1.0f;

	/// <summary>
	/// Recalculates the view matrix from camera parameters.
	/// </summary>
	void update()
	{
		auto rotation = math::rotate(xRotation, { 0.0f, -1.0f, 0.0f })
			* math::rotate(yRotation, { -1.0f, 0.0f, 0.0f });

		translation = 
			math::translate(center)
			* rotation
			* math::scale({ 1.0f, 1.0f, zoomScale })
			* math::translate(glm::vec3{ currentEye, 0.0f, 0.0f })
			* glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

		right = rotation * glm::vec4(initialRight, 1.0f);
		up = rotation * glm::vec4(initialUp, 1.0f);
		front = rotation * glm::vec4(initialFront, 1.0f);

		view = math::lookAt(translation, center, up);

	}

	/// <summary>
	/// Creates a perspective frustum matrix for stereoscopic rendering.
	/// </summary>
	/// <param name="eyeOff">The offset of the eye from the center.</param>
	/// <param name="sgn">A sign multiplier (-1 for left eye, +1 for right eye).</param>
	/// <returns>A glm::mat4 representing the computed frustum matrix.</returns>
	glm::mat4 frustum(float eyeOff, float sgn) const
	{
		float fovtan = std::tan(fov / 2.0f);	

		float a = aspect * fovtan * projectionPlaneDist;
		float b = a + sgn * eyeOff;
		float c = a - sgn * eyeOff;
		float left = -b * near / projectionPlaneDist;
		float right = c * near / projectionPlaneDist;
		float top = near * fovtan;
		
		return math::frustum(left, right, -top, top, near, far);
	}
};