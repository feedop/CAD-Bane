export module math:matrix;

import glm;

/// <summary>
/// Contains various math functions and classes
/// </summary>
export namespace math
{
	inline constexpr double eps = 1e-6;
	inline constexpr float pi = glm::pi<float>();

	/// <summary>
	/// Returns a rotation matrix that rotates a point around a specified axis by a given angle.
	/// </summary>
	/// <param name="angle">The angle (in radians) by which the point will be rotated.</param>
	/// <param name="vector">A glm::vec3 representing the axis of rotation. The vector will be normalized internally.</param>
	/// <returns>
	/// A glm::mat4 matrix that represents the rotation transformation.
	/// </returns>
	glm::mat4 rotate(float angle, const glm::vec3& vector)
	{
		const float cos = glm::cos(angle);
		const float sin = glm::sin(angle);

		glm::vec3 axis = glm::normalize(vector);
		glm::vec3 temp = (1.0f - cos) * axis;

		return glm::mat4{
			cos + temp[0] * axis[0], temp[0] * axis[1] + sin * axis[2], temp[0] * axis[2] - sin * axis[1], 0,
			temp[1] * axis[0] - sin * axis[2], cos + temp[1] * axis[1], temp[1] * axis[2] + sin * axis[0], 0,
			temp[2] * axis[0] + sin * axis[1], temp[2] * axis[1] - sin * axis[0], cos + temp[2] * axis[2], 0,
			0, 0, 0, 1
		};
	}

	/// <summary>
	/// Returns a translation matrix that translates a point by the specified vector.
	/// </summary>
	/// <param name="vector">A glm::vec3 representing the translation vector.</param>
	/// <returns>
	/// A glm::mat4 matrix that represents the translation transformation.
	/// </returns>
	glm::mat4 translate(const glm::vec3& vector)
	{
		glm::mat4 result{ 1.0f };
		result[3] = { vector[0],vector[1],vector[2], 1.0f };
		return result;
	}

	/// <summary>
	/// Returns a scaling matrix that scales a point by the specified vector.
	/// </summary>
	/// <param name="vector">A glm::vec3 representing the scaling factors for the x, y, and z axes.</param>
	/// <returns>
	/// A glm::mat4 matrix that represents the scaling transformation.
	/// </returns>
	glm::mat4 scale(const glm::vec3& vector)
	{
		return glm::diagonal4x4(glm::vec4(vector, 1.0f));
	}

	/// <summary>
	/// Constructs a view matrix that simulates a camera looking from the 'eye' position towards the 'center', with the specified 'up' direction.
	/// </summary>
	/// <param name="eye">A glm::vec3 representing the position of the camera (viewer's eye).</param>
	/// <param name="center">A glm::vec3 representing the target point that the camera is looking at.</param>
	/// <param name="up">A glm::vec3 representing the up direction, which defines the orientation of the camera.</param>
	/// <returns>
	/// A glm::mat4 matrix that represents the **view** transformation, aligning the camera with the specified eye, center, and up direction.
	/// </returns>
	glm::mat4 lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
	{
		const glm::vec3 front(normalize(center - eye));
		const glm::vec3 right(normalize(cross(front, up)));
		const glm::vec3 upnorm(cross(right, front));

		glm::mat4 result{ 1.0f };
		result[0][0] = right.x;
		result[1][1] = right.y;
		result[2][0] = right.z;
		result[0][1] = upnorm.x;
		result[1][1] = upnorm.y;
		result[2][1] = upnorm.z;
		result[0][2] = -front.x;
		result[1][2] = -front.y;
		result[2][2] = -front.z;
		result[3][0] = -dot(right, eye);
		result[3][1] = -dot(upnorm, eye);
		result[3][2] = dot(front, eye);
		return result;
		return glm::lookAt(eye, center, up);
	}

	/// <summary>
	/// Generates a perspective projection matrix based on the provided field of view (FOV), aspect ratio, near and far clipping planes.
	/// </summary>
	/// <param name="fov">The field of view angle in radians, typically in the vertical direction (e.g., 45 degrees = PI/4 radians).</param>
	/// <param name="aspect">The aspect ratio of the viewport (width divided by height).</param>
	/// <param name="zNear">The distance to the near clipping plane, where objects closer than this distance are not rendered.</param>
	/// <param name="zFar">The distance to the far clipping plane, where objects further than this distance are not rendered.</param>
	/// <returns>
	/// A glm::mat4 matrix representing the **perspective projection** transformation, which maps 3D coordinates to 2D screen space 
	/// with a perspective distortion (objects further away appear smaller).
	/// </returns>
	glm::mat4 perspective(float fov, float aspect, float zNear, float zFar)
	{
		const float tanHalfFov = glm::tan(fov / 2.0f);

		glm::mat4 result{ 0.0f };
		result[0][0] = 1.0f / (aspect * tanHalfFov);
		result[1][1] = 1.0f / (tanHalfFov);
		result[2][2] = -(zFar + zNear) / (zFar - zNear);
		result[2][3] = -1.0f;
		result[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
		return result;
	}

	/// <summary>
	/// Generates a frustum projection matrix for a custom view frustum, defined by the left, right, bottom, top, near, and far planes.
	/// </summary>
	/// <param name="left">The x-coordinate of the left vertical clipping plane.</param>
	/// <param name="right">The x-coordinate of the right vertical clipping plane.</param>
	/// <param name="bottom">The y-coordinate of the bottom horizontal clipping plane.</param>
	/// <param name="top">The y-coordinate of the top horizontal clipping plane.</param>
	/// <param name="near">The distance to the near clipping plane (objects closer than this distance are not rendered).</param>
	/// <param name="far">The distance to the far clipping plane (objects further than this distance are not rendered).</param>
	/// <returns>
	/// A glm::mat4 matrix representing a **frustum projection** transformation, which maps 3D coordinates to 2D screen space 
	/// using a  projection, where the view frustum is shaped based on the specified planes.
	/// </returns>
	glm::mat4 frustum(float left, float right, float bottom, float top, float near, float far)
	{
		glm::mat4 result{ 0.0f };
		result[0][0] = (2.0f * near) / (right - left);
		result[1][1] = (2.0f * near) / (top - bottom);
		result[2][0] = (right + left) / (right - left);
		result[2][1] = (top + bottom) / (top - bottom);
		result[2][2] = -(far + near) / (far - near);
		result[2][3] = -1.0f;
		result[3][2] = -(2.0f * far * near) / (far - near);
		return result;
	}
}