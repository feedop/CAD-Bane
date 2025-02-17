export module raycastingstrategy;

import camera;
import ellipsoid;
import shader;

/// <summary>
/// Abstract base class for raycasting strategies used in rendering or simulation systems.
/// This class provides an interface for updating the raycasting process and retrieving the texture ID.
/// </summary>
export class RaycastingStrategy
{
public:
	/// <summary>
	/// Virtual method to update the raycasting strategy based on the current width, height, and shininess.
	/// Derived classes should implement this method to provide specific raycasting logic.
	/// </summary>
	/// <param name="width">The width of the rendering area or simulation domain.</param>
	/// <param name="height">The height of the rendering area or simulation domain.</param>
	/// <param name="lightM">Phong shininess.</param>
	virtual void update(int width, int height, int lightM) = 0;

	/// <summary>
	/// Gets the texture ID associated with the raycasting strategy.
	/// </summary>
	/// <returns>The texture ID used by the raycasting strategy.</returns>
	inline unsigned int getTexId() const
	{
		return texId;
	}

protected:
	/// <summary>
	/// Protected constructor to initialize the raycasting strategy with a camera and ellipsoid.
	/// </summary>
	/// <param name="camera">The camera used to view the scene or simulation.</param>
	/// <param name="ellipsoid">The ellipsoid representing the environment or geometry for raycasting.</param>
	RaycastingStrategy(const Camera& camera, const Ellipsoid& ellipsoid) : camera(camera), ellipsoid(ellipsoid)
	{}

	unsigned int texId = 0;

	const Camera& camera;
	const Ellipsoid& ellipsoid;
};