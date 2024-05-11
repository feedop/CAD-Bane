export module shape;

import drawable;
import selectable;
import updatable;

export class Shape : public Drawable, public Selectable, public Updatable
{
public:
	Shape(const std::string& name) : Selectable(name)
	{}
};