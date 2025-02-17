export module updatable;

import point;

/// <summary>
/// A base class for objects that can be updated. Provides functionality to schedule updates and track the invoker 
/// of the update. Derived classes must implement the <see cref="update"/> method to define the update behavior.
/// </summary>
export class Updatable
{
public:
	/// <summary>
	/// A pure virtual function that must be implemented by derived classes to define the update behavior.
	/// </summary>
	virtual void update() = 0;

	// <summary>
	/// Schedules this object to be updated in the next frame. Optionally, an invoker can be passed to specify which object triggered 
	/// the update.
	/// </summary>
	/// <param name="invoker">An optional pointer to the object that invoked the update. Default is nullptr.</param>
	void scheduleToUpdate(const Point* invoker = nullptr)
	{
		scheduledToUpdate = true;
		if (invoker)
			updateInvoker = invoker;
	}

	virtual ~Updatable() {}

protected:
	bool scheduledToUpdate = true;
	const Point* updateInvoker = nullptr;
};