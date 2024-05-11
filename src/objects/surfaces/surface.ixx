export module surface;

import <glad/glad.h>;
import <memory>;
import <vector>;
import <glm/vec3.hpp>;
import <glm/mat4x4.hpp>;

import patch;
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

	virtual void draw(const Shader* shader) const override
	{
		// patches
		setColor(shader);
		for (auto&& patch : patches)
		{
			patch->draw(shader);
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

	void drawPolygon(const Shader* shader) const
	{
		for (auto&& patch : patches)
		{
			patch->drawPolygon(shader);
		}
	}

	inline const std::vector<Point*>& getPoints() const
	{
		return points;
	}

protected:
	static constexpr float patchSizeX = 1.0f;
	static constexpr float patchSizeZ = 1.0f;

	std::vector<Point*> points;

	std::vector<std::unique_ptr<Patch>> patches;

	Surface(const std::string& surfaceName) : Shape(surfaceName)
	{
		
	}

	virtual std::string getSurfaceName() const = 0;

private:
	friend class GuiController;

	float densityX = 4.0f;
	float densityZ = 4.0f;
};