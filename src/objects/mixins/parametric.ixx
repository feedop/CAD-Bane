export module parametric;

import glm;

import <glad/glad.h>;

import shader;

export
{
	class IntersectionCurve;
	class Parametric;
	class Scene;
	void removeSurfaceFromCurve(IntersectionCurve* curve, Parametric* surface);
	unsigned int getTrimmingTextureFromCurve(IntersectionCurve* curve, const Parametric* surface);
}

export enum class TrimMode : int
{
	Show,
	Hide,
	RemoveWhite,
	RemoveBlack
};

export class Parametric
{
public:
	virtual ~Parametric()
	{
		if (intersectionCurve != nullptr)
			removeSurfaceFromCurve(intersectionCurve, this);
	}

	virtual glm::vec3 evaluate(float u, float v, float toolRadius) const = 0;

	virtual glm::vec3 derivativeU(float u, float v, float toolRadius) const = 0;

	virtual glm::vec3 derivativeV(float u, float v, float toolRadius) const = 0;

	virtual constexpr float getRange() const
	{
		return 1.0f;
	}

	void addIntersection(IntersectionCurve* curve)
	{
		intersectionCurve = curve;
	}

	void removeIntersection(IntersectionCurve* curve)
	{
		if (curve == nullptr || curve == intersectionCurve)
			intersectionCurve = nullptr;

		trimMode = TrimMode::Show;
	}

	inline bool hasIntersection() const
	{
		return intersectionCurve != nullptr;
	}

protected:
	void uploadTrimTexture(const Shader* shader) const
	{
		shader->setInt("trimMode", static_cast<int>(trimMode));
		if (hasIntersection())
			glBindTexture(GL_TEXTURE_2D, getTrimmingTextureFromCurve(intersectionCurve, this));
	}

private:
	friend void renderContextMenu(auto object, Scene* scene);
	IntersectionCurve* intersectionCurve;
	TrimMode trimMode = TrimMode::Show;
};