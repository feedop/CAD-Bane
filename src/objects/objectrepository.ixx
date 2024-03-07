export module objectrepository;

import <memory>;

import <glm/vec3.hpp>;
import <glm/mat4x4.hpp>;
import <glm/gtc/constants.hpp>;

import object;
import canvas;
import cube;
import math;

export class ObjectRepository
{
public:
	inline const Object* getGround() const
	{
		return ground.get();
	}

private:
	const std::unique_ptr<Object> ground = std::make_unique<Canvas>(math::scale({10.0f, 10.0f, 10.0f}) * math::rotate(-math::pi / 2, { 1.0f, 0.0, 0.0 }));
};