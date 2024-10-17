export module surface;

import std;

import <glad/glad.h>;

import <glm/vec3.hpp>;
import <glm/mat4x4.hpp>;
import <Serializer/Serializer.h>;

import parametric;
import pointrenderer;
import shape;
import shader;
import virtualpoint;

export class Surface : public Shape, public Parametric
{
public:
	inline float getDensityX() const
	{
		return densityX;
	}

	inline float getDensityZ() const
	{
		return densityZ;
	}

	void translate(const glm::vec3& positionChange)
	{
		for (auto&& point : points)
		{
			point->translate(positionChange);
		}
	}

	virtual void drawPolygon(const Shader* shader) const = 0;

	virtual void addToMGScene(MG1::Scene& mgscene, const std::vector<std::unique_ptr<Point>>& allPoints) const = 0;

	inline const std::vector<Point*>& getPoints() const
	{
		return points;
	}

	inline bool isCylinder() const
	{
		return cylinder;
	}
	inline int getSizeX() const
	{
		return sizeX;
	}
	inline int getSizeZ() const
	{
		return sizeZ;
	}

	virtual Shader* getPreferredShader() const = 0;

	virtual glm::vec3 evaluate(float u, float v) const override
	{
		return { 0, 0, 0 };
	}

	virtual glm::vec3 derivativeU(float u, float v) const override
	{
		return { 0, 0, 1 };
	}

	virtual glm::vec3 derivativeV(float u, float v) const override
	{
		return { 1, 0, 0 };
	}

protected:
	static constexpr float patchSizeX = 1.0f;
	static constexpr float patchSizeZ = 1.0f;

	float densityX = 4.0f;
	float densityZ = 4.0f;

	int sizeX, sizeZ;
	bool cylinder;

	Surface(const std::string& surfaceName, int sizeX, int sizeZ, bool cylinder = false) : Shape(surfaceName), sizeX(sizeX), sizeZ(sizeZ), cylinder(cylinder)
	{
		
	}

	virtual std::string getSurfaceName() const = 0;

private:
	friend class GuiController;
};