#include "Collisions.h"

#include"Application.h"

#include "ModuleRender.h"
#include "ModuleInput.h"
#include "Physics.h"

#include "SDL/include/SDL_Scancode.h"

Collisions::Collisions(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;


	matrix[Collider::Type::PLAYER][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::GROUND] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::BALL] = true;

	matrix[Collider::Type::GROUND][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::GROUND][Collider::Type::GROUND] = false;
	matrix[Collider::Type::GROUND][Collider::Type::BALL] = true;

	matrix[Collider::Type::BALL][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::BALL][Collider::Type::GROUND] = true;
	matrix[Collider::Type::BALL][Collider::Type::BALL] = true;
}

// Destructor
Collisions::~Collisions()
{

}

update_status Collisions::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->pendingToDelete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// Skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// Avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// Skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (matrix[c1->type][c2->type] && c1->Intersects(c2->rect))
			{
				for (uint i = 0; i < MAX_LISTENERS; ++i)
					if (c1->listeners[i] != nullptr) c1->listeners[i]->OnCollision(c1, c2);

				for (uint i = 0; i < MAX_LISTENERS; ++i)
					if (c2->listeners[i] != nullptr) c2->listeners[i]->OnCollision(c2, c1);
			}
		}
	}

	return UPDATE_CONTINUE;
}

update_status Collisions::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		debug = !debug;

	return UPDATE_CONTINUE;
}

bool Collisions::PostUpdate()
{
	if (debug) DebugDraw();

	return UPDATE_CONTINUE;
}

void Collisions::DebugDraw()
{
	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case Collider::Type::NONE: // white
			app->render->DrawRectangle({ METERS_TO_PIXELS(colliders[i]->rect.x), METERS_TO_PIXELS(colliders[i]->rect.y), METERS_TO_PIXELS(colliders[i]->rect.w), METERS_TO_PIXELS(colliders[i]->rect.w) }, 255, 255, 255, alpha);

			//	app->render->DrawRectangle(colliders[i]->rect, 255, 255, 255, alpha );
			break;
		case Collider::Type::BALL: // blue
			app->render->DrawRectangle({ METERS_TO_PIXELS(colliders[i]->rect.x), METERS_TO_PIXELS(colliders[i]->rect.y), METERS_TO_PIXELS(colliders[i]->rect.w), METERS_TO_PIXELS(colliders[i]->rect.w) }, 255, 0, 0, alpha);
			break;
		case Collider::Type::PLAYER: // green
			app->render->DrawRectangle({ METERS_TO_PIXELS(colliders[i]->rect.x), METERS_TO_PIXELS(colliders[i]->rect.y), METERS_TO_PIXELS(colliders[i]->rect.w), METERS_TO_PIXELS(colliders[i]->rect.w) }, 0, 0, 255, alpha);
			break;
		case Collider::Type::GROUND: // red
			app->render->DrawRectangle({ METERS_TO_PIXELS(colliders[i]->rect.x), METERS_TO_PIXELS(colliders[i]->rect.y), METERS_TO_PIXELS(colliders[i]->rect.w), METERS_TO_PIXELS(colliders[i]->rect.h) }, 0, 255, 0, alpha);
			break;
		}
	}
}

// Called before quitting
bool Collisions::CleanUp()
{
	//LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* Collisions::AddCollider(SDL_Rect_Double rect, Collider::Type type, Module* listener)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, listener);
			break;
		}
	}

	return ret;
}

void Collisions::RemoveCollider(Collider* collider)
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == collider)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}
}