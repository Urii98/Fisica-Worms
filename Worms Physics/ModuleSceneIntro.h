#pragma once
#include "Module.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "ModulePhysics.h"

#define BOUNCER_TIME 200

struct SDL_Texture;

struct Bouncer
{
	Bouncer() : body(NULL), texture(NULL), hit_timer(0), fx(0)
	{}

	PhysBody* body;
	SDL_Texture* texture;
	Uint32 hit_timer;
	uint fx;
};

enum lightTypes
{
	tiny,
	medium,
	big
};

class ModuleSceneIntro;

struct Light
{
	Light() : body(NULL), texture(NULL), on(false), fx(0)
	{}

	Light(ModuleSceneIntro* physics, int x, int y, lightTypes type);

	lightTypes type;
	PhysBody* body;
	SDL_Texture* texture;
	bool on;
	uint fx;
	int x, y;
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	SDL_Texture* graphics;
	PhysBody* background;

	Bouncer bouncer1;
	Bouncer bouncer2;

	Bouncer side_bouncer1;
	Bouncer side_bouncer2;

	SDL_Texture* tex_light_tiny;
	SDL_Texture* tex_light_medium;
	SDL_Texture* tex_light_big;
	
	uint fx_light_tiny;
	uint fx_light_medium;
	uint fx_light_big;

	p2DynArray<Light> lights;

	PhysBody* player_lose;
	uint player_lose_fx;

	
	// Physics objects
	std::vector<PhysBall> balls{};
	Atmosphere atmosphere{};
	Ground ground{};

	// Base walls
	Wall wall1{};
	Wall wall2{};

	// Right basket
	Wall wall3{};
	Wall wall4{};
	Wall wall5{};

	// Center basket
	Wall wall6{};
	Wall wall7{};
	Wall wall8{};

	// Left basket
	Wall wall9{};
	Wall wall10{};
	Wall wall11{};

	// Left bouncy wall
	Wall wall12{};

	// Right bouncy wall
	Wall wall13{};


	// Bounce wall 1

	Water water1{};
	Water water2{};

	
};
