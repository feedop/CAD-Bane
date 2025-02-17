export module math:quat;

import std;
import glm;

export namespace math
{
	/// <summary>
	/// A class representing a quaternion for 3D rotations. This class supports various operations on quaternions, including 
	/// multiplication, normalization, conversion to matrix form, and more. Quaternions are often used in 3D graphics and physics 
	/// to represent rotations because they avoid issues like gimbal lock that can occur with Euler angles.
	/// </summary>
	class Quat
	{
	private:
		float w = 1.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

	public:
		/// <summary>
		/// Default constructor for a quaternion. Initializes to the identity quaternion (w = 1, x = 0, y = 0, z = 0).
		/// </summary>
		Quat() = default;

		/// <summary>
		/// Constructs a quaternion from the given components.
		/// </summary>
		/// <param name="w">The scalar component of the quaternion.</param>
		/// <param name="x">The x-component of the vector part of the quaternion.</param>
		/// <param name="y">The y-component of the vector part of the quaternion.</param>
		/// <param name="z">The z-component of the vector part of the quaternion.</param>
		Quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z)
		{}

		/// <summary>
		/// Constructs a quaternion from a scalar and a 3D vector.
		/// </summary>
		/// <param name="w">The scalar component of the quaternion.</param>
		/// <param name="xyz">The 3D vector part of the quaternion.</param>
		Quat(float w, const glm::vec3& xyz) : Quat(w, xyz.x, xyz.y, xyz.z)
		{}

		/// <summary>
		/// Constructs a quaternion from a 4D vector (w, x, y, z).
		/// </summary>
		/// <param name="wxyz">The 4D vector representing the quaternion components.</param>
		Quat(const glm::vec4& wxyz) : Quat(wxyz.w, wxyz.x, wxyz.y, wxyz.z)
		{}

		/// <summary>
		/// Constructs a quaternion from Euler angles (roll, pitch, yaw).
		/// </summary>
		/// <param name="rpy">The Euler angles (roll, pitch, yaw) in radians.</param>
		Quat(const glm::vec3& rpy)
		{
			glm::vec3 c = glm::cos(rpy * 0.5f);
			glm::vec3 s = glm::sin(rpy * 0.5f);

			this->w = c.x * c.y * c.z + s.x * s.y * s.z;
			this->x = s.x * c.y * c.z - c.x * s.y * s.z;
			this->y = c.x * s.y * c.z + s.x * c.y * s.z;
			this->z = c.x * c.y * s.z - s.x * s.y * c.z;
		}

		/// <summary>
		/// Multiplies this quaternion with another quaternion and returns the result.
		/// </summary>
		/// <param name="other">The other quaternion to multiply with.</param>
		/// <returns>A new quaternion resulting from the multiplication of this and the other quaternion.</returns>
		Quat operator*(const Quat& other) const
		{
			float newW = w * other.w - x * other.x - y * other.y - z * other.z;
			float newX = w * other.x + x * other.w + y * other.z - z * other.y;
			float newY = w * other.y + y * other.w + z * other.x - x * other.z;
			float newZ = w * other.z + z * other.w + x * other.y - y * other.x;
			return Quat(newW, newX, newY, newZ);
		}

		/// <summary>
		/// Multiplies this quaternion with another quaternion in-place.
		/// </summary>
		/// <param name="other">The other quaternion to multiply with.</param>
		/// <returns>This quaternion after the multiplication.</returns>
		Quat& operator*=(const Quat& other)
		{
			*this *= other;
			return *this;
		}

		/// <summary>
		/// Converts the quaternion to a 4x4 matrix.
		/// </summary>
		/// <returns>A 4x4 matrix representing the rotation of this quaternion.</returns>
		glm::mat4 toMatrix() const
		{
			glm::mat3 result{ 1.0f };
			float qxx = x * x;
			float qyy = y * y;
			float qzz = z * z;
			float qxz = x * z;
			float qxy = x * y;
			float qyz = y * z;
			float qwx = w * x;
			float qwy = w * y;
			float qwz = w * z;

			result[0][0] = 1.0f - 2.0f * (qyy + qzz);
			result[0][1] = 2.0f * (qxy + qwz);
			result[0][2] = 2.0f * (qxz - qwy);

			result[1][0] = 2.0f * (qxy - qwz);
			result[1][1] = 1.0f - 2.0f * (qxx + qzz);
			result[1][2] = 2.0f * (qyz + qwx);

			result[2][0] = 2.0f * (qxz + qwy);
			result[2][1] = 2.0f * (qyz - qwx);
			result[2][2] = 1.0f - 1.0f * (qxx + qyy);

			return glm::mat4(result);
		}

		/// <summary>
		/// Returns the length (magnitude) of the quaternion.
		/// </summary>
		/// <returns>The length of the quaternion.</returns>
		float length() const
		{
			return std::sqrtf(w * w + x * x + y * y + z * z);
		}

		/// <summary>
		/// Returns a normalized version of this quaternion.
		/// </summary>
		/// <returns>The normalized quaternion.</returns>
		Quat normalized() const
		{
			float len = length();
			if (len <= 0.0f) [[unlikely]]
				return Quat(1.0f, 0.0f, 0.0f, 0.0f);
			float oneOverLen = 1.0f / len;
			return Quat(w * oneOverLen, x * oneOverLen, y * oneOverLen, z * oneOverLen);
		}

		/// <summary>
		/// Converts the quaternion to Euler angles (roll, pitch, yaw).
		/// </summary>
		/// <returns>A 3D vector representing the Euler angles (roll, pitch, yaw) in radians.</returns>
		glm::vec3 rpy() const
		{
			return glm::vec3{ pitch(), yaw(), roll() };
		}

		/// <summary>
		/// Creates a quaternion from an axis and an angle of rotation.
		/// </summary>
		/// <param name="angle">The angle of rotation in radians.</param>
		/// <param name="axis">The axis of rotation represented as a 3D vector.</param>
		/// <returns>A quaternion representing the rotation.</returns>
		static Quat angleAxis(float angle, const glm::vec3& axis)
		{
			float const a(angle);
			float const s = glm::sin(a * 0.5f);

			return Quat(glm::cos(a * 0.5f), axis * s);
		}

	private:
		// <summary>
		/// Calculates the roll (rotation around the x-axis) from the quaternion.
		/// </summary>
		/// <returns>The roll angle in radians.</returns>
		float roll() const
		{
			float yy = 2.0f * (x * y + w * z);
			float xx = w * w + x * x - y * y - z * z;

			if (glm::length(glm::vec2{ xx, yy }) < 1e-6) [[unlikely]] //avoid atan2(0,0) - handle singularity
				return 0.0f;

			return atan2(yy, xx);
		}

		/// <summary>
		/// Calculates the pitch (rotation around the y-axis) from the quaternion.
		/// </summary>
		/// <returns>The pitch angle in radians.</returns>
		float pitch() const
		{
			float yy = 2.0f * (y * z + w * x);
			float xx = w * w - x * x - y * y + z * z;

			if (glm::length(glm::vec2{ xx, yy }) < 1e-6) [[unlikely]] //avoid atan2(0,0) - handle singularity
				return 0.0f;

			return atan2(yy, xx);
		}

		/// <summary>
		/// Calculates the yaw (rotation around the z-axis) from the quaternion.
		/// </summary>
		/// <returns>The yaw angle in radians.</returns>
		float yaw() const
		{
			return asin(std::clamp(-2.0f * (x * z - w * y), -1.0f, 1.0f));
		}
	};
}