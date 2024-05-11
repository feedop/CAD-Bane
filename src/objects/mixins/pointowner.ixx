export module pointowner;

import <memory>;
import <vector>;

import point;
import shader;

export class PointOwner
{
public:
	virtual void drawAdditionalPoints(const Shader* shader) const
	{

	}

	virtual const std::vector<std::unique_ptr<Point>>& getVirtualPoints() const
	{
		return empty;
	}

	virtual void updateRenderer()
	{

	}

	virtual ~PointOwner() {}

private:
	inline static const std::vector<std::unique_ptr<Point>> empty;
};