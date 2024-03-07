export module math:phong;

import <glm/geometric.hpp>;
import <glm/vec3.hpp>;
import <glm/mat4x4.hpp>;


export namespace math
{
	float specular(const glm::mat4& Dm, const glm::vec3& pos, int m)
	{
		auto toCamera = glm::normalize(glm::vec3{0, 0, 10} - pos);

		float dx = Dm[0][3] + Dm[3][0] + 2 * Dm[0][0] * pos.x + Dm[0][1] * pos.y + Dm[1][0] * pos.y + Dm[0][2] * pos.z + Dm[2][0] * pos.z;
		float dy = Dm[1][3] + Dm[3][1] + Dm[0][1] * pos.x + Dm[1][0] * pos.x + 2 * Dm[1][1] * pos.y + Dm[1][2] * pos.z + Dm[2][1] * pos.z;
		float dz = Dm[2][3] + Dm[3][2] + Dm[0][2] * pos.x + Dm[2][0] * pos.x + Dm[1][2] * pos.y + Dm[2][1] * pos.y + 2 * Dm[2][2] * pos.z;

		auto normal = glm::normalize(glm::vec3{dx, dy, dz});
		return std::pow(std::max(glm::dot(toCamera, normal), 0.0f), m);
	}
}