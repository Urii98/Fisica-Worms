#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// Create ground
	ground = Ground();
	ground.x = 17.0f; // [m]
	ground.y = 0.0f; // [m]
	ground.w = 16.0f; // [m]
	ground.h = 10.0f; // [m]

	// Create walls
	wall1 = Wall();
	wall1.x = 12.f; // [m]
	wall1.y = 0.0f; // [m]
	wall1.w = 5.0f; // [m]
	wall1.h = 15.0f; // [m]
	wall1.bouncyness = 1.0f;

	wall2 = Wall();
	wall2.x = ground.x + ground.w; // [m]
	wall2.y = 0.0f; // [m]
	wall2.w = 5.0f; // [m]
	wall2.h = 15.0f; // [m]
	wall2.bouncyness = 1.0f;

	// Right Basket
	wall3 = Wall();
	wall3.x = 42.0f; // [m]
	wall3.y = 18.0f; // [m]
	wall3.w = 0.5f; // [m]
	wall3.h = 3.0f; // [m]
	wall3.bouncyness = 1.0f;

	wall4 = Wall();
	wall4.w = 5.0f; // [m]
	wall4.h = 0.5f; // [m]
	wall4.x = wall3.x + wall4.h; // [m]
	wall4.y = wall3.y - wall4.h; // [m]
	wall4.bouncyness = 1.0f;

	wall5 = Wall();
	wall5.x = wall4.x + wall4.w; // [m]
	wall5.y = wall3.y; // [m]
	wall5.w = wall3.w; // [m]
	wall5.h = wall3.h; // [m]
	wall5.bouncyness = 1.0f;

	// Center Basket
	wall6 = Wall();
	wall6.x = 21.0f; // [m]
	wall6.y = 30.0f; // [m]
	wall6.w = 0.5f; // [m]
	wall6.h = 3.0f; // [m]
	wall6.bouncyness = 1.0f;

	wall7 = Wall();
	wall7.w = 8.0f; // [m]
	wall7.h = 0.5f; // [m]
	wall7.x = wall6.x + wall7.h; // [m]
	wall7.y = wall6.y - wall7.h; // [m]
	wall7.bouncyness = 1.0f;

	wall8 = Wall();
	wall8.x = wall7.x + wall7.w; // [m]
	wall8.y = wall6.y; // [m]
	wall8.w = wall6.w; // [m]
	wall8.h = wall6.h; // [m]
	wall8.bouncyness = 1.0f;

	// Left Basket
	wall9 = Wall();
	wall9.x = 3.0f; // [m]
	wall9.y = 18.0f; // [m]
	wall9.w = 0.5f; // [m]
	wall9.h = 3.0f; // [m]
	wall9.bouncyness = 1.0f;

	wall10 = Wall();
	wall10.w = 5.0f; // [m]
	wall10.h = 0.5f; // [m]
	wall10.x = wall9.x + wall10.h; // [m]
	wall10.y = wall9.y - wall10.h; // [m]
	wall10.bouncyness = 1.0f;

	wall11 = Wall();
	wall11.x = wall10.x + wall10.w; // [m]
	wall11.y = wall9.y; // [m]
	wall11.w = wall9.w; // [m]
	wall11.h = wall9.h; // [m]
	wall11.bouncyness = 1.0f;

	// Left Bouncy Wall
	wall12 = Wall();
	wall12.x = wall1.x; // [m]
	wall12.y = 25.0f; // [m]
	wall12.w = 0.5f; // [m]
	wall12.h = 3.0f; // [m]
	wall12.bouncyness = 2.0f;

	// Right Bouncy Wall
	wall13 = Wall();
	wall13.x = wall2.x + wall2.w; // [m]
	wall13.y = 25.0f; // [m]
	wall13.w = 0.5f; // [m]
	wall13.h = 3.0f; // [m]
	wall13.bouncyness = 2.0f;

	walls.emplace_back(wall1);
	walls.emplace_back(wall2);
	walls.emplace_back(wall3);
	walls.emplace_back(wall4);
	walls.emplace_back(wall5);
	walls.emplace_back(wall6);
	walls.emplace_back(wall7);
	walls.emplace_back(wall8);
	walls.emplace_back(wall9);
	walls.emplace_back(wall10);
	walls.emplace_back(wall11);
	walls.emplace_back(wall12);
	walls.emplace_back(wall13);

	// Create Water
	water1 = Water();
	water1.x = 0.0f;
	water1.y = 0.0f; // [m]
	water1.w = 12.0f; // [m]
	water1.h = 5.0f; // [m]
	water1.density = 50.0f; // [kg/m^3]
	water1.vx = 0.0f; // [m/s]
	water1.vy = 0.0f; // [m/s]

	water2 = Water();
	water2.x = wall2.x + wall2.w; // Start where ground ends [m]
	water2.y = 0.0f; // [m]
	water2.w = 15.0f; // [m]
	water2.h = 5.0f; // [m]
	water2.density = 50.0f; // [kg/m^3]
	water2.vx = -1.0f; // [m/s]
	water2.vy = 0.0f; // [m/s]

	// Create atmosphere
	atmosphere = Atmosphere();
	atmosphere.windx = 10.0f; // [m/s]
	atmosphere.windy = 5.0f; // [m/s]
	atmosphere.density = 1.0f; // [kg/m^3]

	// Create a ball
	PhysBall ball = PhysBall();

	// Set static properties of the ball
	ball.mass = 10.0f; // [kg]
	ball.surface = 1.0f; // [m^2]
	ball.radius = 0.5f; // [m]
	ball.cd = 0.4f; // [-]
	ball.cl = 1.2f; // [-]
	ball.b = 10.0f; // [...]
	ball.coef_friction = 0.9f; // [-]
	ball.coef_restitution = 0.8f; // [-]

	// Set initial position and velocity of the ball
	ball.x = 25.0f;
	ball.y = 15.0f;
	ball.vx = 0.0f;
	ball.vy = 0.0f;

	// Add ball to the collection
	balls.emplace_back(ball);

	App->renderer->camera.x = App->renderer->camera.y = 0;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		for (auto& ball : balls)
		{
			ball.vy += 50 * 0.016f;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		for (auto& ball : balls)
		{
			ball.vy -= 50 * 0.016f;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		for (auto& ball : balls)
		{
			ball.vx -= 50 * 0.016f;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		for (auto& ball : balls)
		{
			ball.vx += 50 * 0.016f;
		}
	}
		

	return UPDATE_CONTINUE;
}
