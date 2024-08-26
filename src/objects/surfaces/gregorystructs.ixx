export module gregorystructs;

import <array>;
import <vector>;

import point;

export namespace Gregory
{
	struct HoleNode
	{
		Point* p = nullptr;
		Point* prev = nullptr;
	};

	using HoleEdge = std::array<HoleNode, 4>;
	using Hole = std::array<HoleEdge, 3>;

	struct BorderNode
	{
		Point* p = nullptr;
		Point* prevBefore = nullptr;
		Point* prevAfter = nullptr;

		bool isCorner() const
		{
			return prevBefore != nullptr && prevAfter != nullptr;
		}
	};

	using Border = std::vector<BorderNode>;
}