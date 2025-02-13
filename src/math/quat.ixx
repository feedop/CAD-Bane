export module math:quat;

import std;
import glm;

export namespace math
{
	class Quat
	{
	private:
		float w = 1.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

	public:
		Quat() = default;
		Quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z)
		{}

		Quat(float w, const glm::vec3& xyz) : Quat(w, xyz.x, xyz.y, xyz.z)
		{}

		Quat(const glm::vec4& wxyz) : Quat(wxyz.w, wxyz.x, wxyz.y, wxyz.z)
		{}

		Quat(const glm::vec3& rpy)
		{
			glm::vec3 c = glm::cos(rpy * 0.5f);
			glm::vec3 s = glm::sin(rpy * 0.5f);

			this->w = c.x * c.y * c.z + s.x * s.y * s.z;
			this->x = s.x * c.y * c.z - c.x * s.y * s.z;
			this->y = c.x * s.y * c.z + s.x * c.y * s.z;
			this->z = c.x * c.y * s.z - s.x * s.y * c.z;
		}

		Quat operator*(const Quat& other) const
		{
			float newW = w * other.w - x * other.x - y * other.y - z * other.z;
			float newX = w * other.x + x * other.w + y * other.z - z * other.y;
			float newY = w * other.y + y * other.w + z * other.x - x * other.z;
			float newZ = w * other.z + z * other.w + x * other.y - y * other.x;
			return Quat(newW, newX, newY, newZ);
		}

		Quat& operator*=(const Quat& other)
		{
			*this *= other;
			return *this;
		}

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

		float length() const
		{
			return std::sqrtf(w * w + x * x + y * y + z * z);
		}

		Quat normalized() const
		{
			float len = length();
			if (len <= 0.0f) [[unlikely]]
				return Quat(1.0f, 0.0f, 0.0f, 0.0f);
			float oneOverLen = 1.0f / len;
			return Quat(w * oneOverLen, x * oneOverLen, y * oneOverLen, z * oneOverLen);
		}

		glm::vec3 rpy() const
		{
			return glm::vec3{ pitch(), yaw(), roll() };
		}

		static Quat angleAxis(float angle, const glm::vec3& axis)
		{
			float const a(angle);
			float const s = glm::sin(a * 0.5f);

			return Quat(glm::cos(a * 0.5f), axis * s);
		}

	private:
		float roll() const
		{
			float yy = 2.0f * (x * y + w * z);
			float xx = w * w + x * x - y * y - z * z;

			if (glm::length(glm::vec2{ xx, yy }) < 1e-6) [[unlikely]] //avoid atan2(0,0) - handle singularity
				return 0.0f;

			return atan2(yy, xx);
		}

		float pitch() const
		{
			float yy = 2.0f * (y * z + w * x);
			float xx = w * w - x * x - y * y + z * z;

			if (glm::length(glm::vec2{ xx, yy }) < 1e-6) [[unlikely]] //avoid atan2(0,0) - handle singularity
				return 0.0f;

			return atan2(yy, xx);
		}

		float yaw() const
		{
			return asin(std::clamp(-2.0f * (x * z - w * y), -1.0f, 1.0f));
		}
	};
}