#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "PhysBody.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	dt = App->physics->dt;

	score = 0;

	body = PhysBall();

	// Set static properties of the ball
	body.mass = 10.0f; // [kg]
	body.surface = 1.0f; // [m^2]
	body.radius = 0.5f; // [m]
	body.cd = 0.4f; // [-]
	body.cl = 1.2f; // [-]
	body.b = 10.0f; // [...]
	body.coef_friction = 0.9f; // [-]
	body.coef_restitution = 0.8f; // [-]

	// Set initial position and velocity of the ball
	body.x = 25.0f;
	body.y = 10.0f;
	body.vx = 0.0f;
	body.vy = 0.0f;

	moveType = VELOCITY;

	isDead = false;
	onGround = true;

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		moveType = POSITION;
		//body.physics_enabled = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		moveType = FORCES;
		body.physics_enabled = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		moveType = VELOCITY;
		body.physics_enabled = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN && onGround)
	{
		body.vy += 1000 * dt;
		onGround = false;
	}

	switch (moveType)
	{
	case NONE:
		break;
	case POSITION:
		/*if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			body.y += 50 * dt;
		}
		else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			body.y -= 50 * dt;
		}*/
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			body.x -= 50 * dt;
		}
		else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			body.x += 50 * dt;
		}
		break;
	case FORCES:
		/*if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			body.AddForce(0, 3000);
		}
		else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			body.AddForce(0, -3000);
		}*/
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			body.AddForce(-500, 0);
		}
		else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			body.AddForce(500, 0);
		}
		break;
	case VELOCITY:
		/*if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			body.vy += 50 * dt;
		}
		else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			body.vy -= 50 * dt;
		}*/
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (body.vx >= -15)
			{
				body.vx = -500 * dt;
			}			
		}
		else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (body.vx <= 15)
			{
				body.vx = 500 * dt;
			}
		}
		break;
	default:
		break;
	}

	//printf("SPEED: %f", body.vx);

	return UPDATE_CONTINUE;
}



