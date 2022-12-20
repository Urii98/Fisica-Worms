#include "Collider.h"

Collider::Collider(SDL_Rect_Double rectangle, Type type, Module* listener) : rect(rectangle), type(type)
{
	listeners[0] = listener;
}

void Collider::SetPos(double x, double y)
{
	rect.x = x;
	rect.y = y;
}

bool Collider::Intersects(const SDL_Rect_Double& r) const
{
	// L6: DONE 1: Check if there is an overlap
	// between argument "r" and property "rect"
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}

void Collider::AddListener(Module* listener)
{
	for (int i = 0; i < MAX_LISTENERS; ++i)
	{
		if (listeners[i] == nullptr)
		{
			listeners[i] = listener;
			break;
		}

		// Simple security check to avoid adding the same listener twice
		else if (listeners[i] == listener) break;
	}
}