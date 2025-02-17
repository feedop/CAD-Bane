export module math:phong;

import std;
import glm;

export namespace math
{
	/// <summary>
	/// Computes the specular reflection component based on the camera position, the normal derived from the deformation matrix, 
	/// and a shininess factor. This function models the brightness of the reflection of light on a surface based on the 
	/// viewing angle and surface roughness.
	/// </summary>
	/// <param name="Dm">A glm::mat4 matrix representing the deformation or transformation matrix. It is used to calculate the 
	/// surface normal based on the position in 3D space.</param>
	/// <param name="pos">A glm::vec3 representing the position in 3D space where the surface normal is evaluated.</param>
	/// <param name="m">An integer representing the shininess factor. The higher the value, the sharper and more intense the 
	/// specular reflection will be.</param>
	/// <returns>
	/// A float representing the specular reflection intensity at the given position, calculated based on the camera's 
	/// direction, the surface normal, and the shininess factor. The value is between 0 and 1, where 1 represents a perfect 
	/// specular reflection and 0 represents no reflection.
	/// </returns>
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