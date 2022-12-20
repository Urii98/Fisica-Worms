#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "Module.h"
#include "SDL/include/SDL_Rect.h"

#define MAX_LISTENERS 5



class Collider: public Module
{
	enum Type
	{
		NONE = -1,
		GROUND,
		PLAYER,
		BALL,

		MAX
	};

	// Methods
	Collider(SDL_Rect_Double rectangle, Type type, Module* listener = nullptr);

	void SetPos(double x, double y);

	bool Intersects(const SDL_Rect_Double& r) const;

	void AddListener(Module* listener);

	// Variables
	SDL_Rect_Double rect;
	bool pendingToDelete = false;
	Type type;
	Module* listeners[MAX_LISTENERS] = { nullptr };
};


#endif // !__COLLIDER_H__


