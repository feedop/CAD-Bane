export module raycastingop;

import raycastingstrategy;

/// <summary>
/// Represents a raycasting operation with specific dimensions.
/// This operation can be executed using a raycasting strategy.
/// </summary>
export class RaycastingOp
{
public:
	/// <summary>
	/// Constructs a RaycastingOp with the given texture width and height.
	/// </summary>
	/// <param name="width">The width of the texture.</param>
	/// <param name="height">The height of the texture.</param>
	RaycastingOp(int width, int height) : width(width), height(height)
	{}

	/// <summary>
	/// Executes the raycasting operation using the specified strategy.
	/// </summary>
	/// <param name="strategy">The raycasting strategy to use (CPU or GPU).</param>
	/// <param name="lightM">The lighting model parameter.</param>
	void execute(RaycastingStrategy& strategy, int lightM) const
	{
		strategy.update(width, height, lightM);
	}

private:
	int width, height;
};