export module gregorystructs;

import std;

import point;

/// <summary>
/// Contains elements related to creating Gregory patches used to fill holes
/// </summary>
export namespace Gregory
{
	/// <summary>
	/// Represents a node in the linked list of a hole, containing a point and its previous point.
	/// </summary>
	struct HoleNode
	{
		Point* p = nullptr;
		Point* prev = nullptr;
	};

	using HoleEdge = std::array<HoleNode, 4>;
	using Hole = std::array<HoleEdge, 3>;

	/// <summary>
	/// Represents a node in a border, containing a point and references to previous points before and after it.
	/// </summary>
	struct BorderNode
	{
		Point* p = nullptr;
		Point* prevBefore = nullptr;
		Point* prevAfter = nullptr;

		/// <summary>
		/// Checks if this node is a corner, i.e., if it has both a previous and next point in the border.
		/// </summary>
		/// <returns>True if both `prevBefore` and `prevAfter` are non-null, indicating a corner node.</returns>
		bool isCorner() const
		{
			return prevBefore != nullptr && prevAfter != nullptr;
		}
	};

	using Border = std::vector<BorderNode>;
}