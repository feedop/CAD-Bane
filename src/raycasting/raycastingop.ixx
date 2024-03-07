export module raycastingop;

import raycastingstrategy;

export class RaycastingOp
{
public:
	RaycastingOp(int width, int height) : width(width), height(height)
	{}
	void execute(RaycastingStrategy& strategy, int lightM) const
	{
		strategy.update(width, height, lightM);
	}

private:
	int width, height;
};