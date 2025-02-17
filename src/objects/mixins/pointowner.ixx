export module pointowner;

import std;

import point;
import shader;

/// <summary>
/// Base class for objects that can manage and render points. Provides methods to draw additional points,
/// access virtual points, and update the renderer associated with the points.
/// </summary>
export class PointOwner
{
public:
	/// <summary>
	/// Draws additional points associated with this object using the provided shader.
	/// This method can be overridden in derived classes to implement custom rendering logic.
	/// </summary>
	/// <param name="shader">The shader to use for rendering the points.</param>
	virtual void drawAdditionalPoints(const Shader* shader) const
	{
		// Empty default
	}

	/// <summary>
	/// Gets the virtual points associated with this object. This method can be overridden in derived classes
	/// to provide specific point data.
	/// </summary>
	/// <returns>A constant reference to a vector of unique pointers to Point objects.</returns>
	virtual const std::vector<std::unique_ptr<Point>>& getVirtualPoints() const
	{
		return empty;
	}

	/// <summary>
	/// Updates the renderer for the points. This method can be overridden to provide custom logic for updating
	/// the rendering system related to the points.
	/// </summary>
	virtual void updateRenderer()
	{
		// Empty default
	}

	virtual ~PointOwner() {}

private:
	inline static const std::vector<std::unique_ptr<Point>> empty;
};