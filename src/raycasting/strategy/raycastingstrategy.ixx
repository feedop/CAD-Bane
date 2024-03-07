export module raycastingstrategy;

import camera;
import ellipsoid;
import shader;

export class RaycastingStrategy
{
public:
	virtual void update(int width, int height, int lightM) = 0;

	inline unsigned int getTexId() const
	{
		return texId;
	}

protected:
	RaycastingStrategy(const Camera& camera, const Ellipsoid& ellipsoid) : camera(camera), ellipsoid(ellipsoid)
	{}

	unsigned int texId = 0;

	const Camera& camera;
	const Ellipsoid& ellipsoid;
};