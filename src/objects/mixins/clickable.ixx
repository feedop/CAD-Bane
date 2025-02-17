export module clickable;

import glm;

/// <summary>
/// An abstract class that represents an object which can be clicked on screen. This class defines a virtual function 
/// <see cref="isCoordInObject"/> that should be implemented by derived classes to determine whether a given 
/// coordinate is inside the object.
/// </summary>
export class Clickable
{
public:
    /// <summary>
    /// A pure virtual function that checks if a given coordinate is within the boundaries of the object.
    /// Derived classes should implement this method to define their own logic for determining whether the 
    /// coordinate is inside the object.
    /// </summary>
    /// <param name="coord">The 3D coordinate to check.</param>
    /// <returns><c>true</c> if the coordinate is inside the object, <c>false</c> otherwise.</returns>
	virtual bool isCoordInObject(const glm::vec3& coord) const = 0;

	virtual ~Clickable() {}
};