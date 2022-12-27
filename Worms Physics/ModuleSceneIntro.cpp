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


	char lookupTableChars[] = { " !'#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[/]^_ abcdefghijklmnopqrstuvwxyz{|}~ ������a��������a��������" };
	textFont = App->fonts->Load("Assets/pixel_font.png", lookupTableChars, 8);
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

	// Create walls
	wall2 = Wall();
	wall2.x = ground.x + ground.w; // [m]
	wall2.y = 0.0f; // [m]
	wall2.w = 5.0f; // [m]
	wall2.h = 15.0f; // [m]
	wall2.bouncyness = 1.0f;

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
	ball.x = 2.0f;
	ball.y = (ground.y + ground.h) + 2.0f;
	ball.vx = 5.0f;
	ball.vy = 10.0f;

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

	App->fonts->BlitText(0, 0, textFont, "Gravity: ");

	App->fonts->BlitText(0, 17, textFont, "Drag: ");

	App->fonts->BlitText(0, 32, textFont, "H. Drag: ");

	App->fonts->BlitText(0, 47, textFont, "H. Buoyancy: ");

	

	return UPDATE_CONTINUE;
}
