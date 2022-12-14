#pragma once
#include "Module.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "ModulePhysics.h"

#define PIXELS_PER_METER (20.0f) // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL (1.0f / PIXELS_PER_METER) // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) std::floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

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
	std::vector<Wall> walls{};
	std::vector<SensorWall> sensorWalls{};
	Atmosphere atmosphere{};
	Ground ground{};

	//Sensor esquerra
	SensorWall sensor1{};

	//Sensor mig
	SensorWall sensor2{};

	//Sensor dreta
	SensorWall sensor3{};

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

	// Water limits
	Wall wall14{};
	Wall wall15{};


	// Bounce wall 1

	std::vector<Water> waters{};
	Water water1{};
	Water water2{};

	
};
