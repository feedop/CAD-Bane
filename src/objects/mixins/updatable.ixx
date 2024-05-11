export module updatable;

import point;

export class Updatable
{
public:
	virtual void update() = 0;

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