export module surface;

import <glad/glad.h>;
import <memory>;
import <vector>;
import <glm/vec3.hpp>;
import <glm/mat4x4.hpp>;
import <Serializer/Serializer.h>;

import pointrenderer;
import shape;
import shader;
import virtualpoint;

export class Surface : public Shape
{
public:
	virtual ~Surface()
	{
		for (auto&& point : points)
		{
			point->detach(this);
		}
	}

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

	virtual Shader* getPreferredShader() const = 0;

protected:
	static constexpr float patchSizeX = 1.0f;
	static constexpr float patchSizeZ = 1.0f;

	float densityX = 4.0f;
	float densityZ = 4.0f;

	std::vector<Point*> points;

	int sizeX = 4, sizeZ = 4;
	bool cylinder;

	Surface(const std::string& surfaceName, int sizeX, int sizeZ, bool cylinder = false) : Shape(surfaceName), sizeX(sizeX), sizeZ(sizeZ), cylinder(cylinder)
	{
		
	}

	void attachPoints()
	{
		for (auto&& point : points)
		{
			point->attach(this);
		}
	}

	virtual std::string getSurfaceName() const = 0;

private:
	friend class GuiController;
};