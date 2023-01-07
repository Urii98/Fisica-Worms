#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"
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

	strength = 3;
	angle_shot = 0.5f;

	body = PhysBall();

	// Set static properties of the ball
	body.mass = 10.0f; // [kg]
	body.surface = 1.0f; // [m^2]
	body.radius = 0.5f; // [m]
	body.cd = 0.4f; // [-]
	body.cl = 1.2f; // [-]
	body.b = 10.0f; // [...]
	body.coef_friction = 0.7f; // [-]
	body.coef_restitution = 0.0f; // [-]

	// Set initial position and velocity of the ball
	body.x = 25.0f;
	body.y = 10.0f;
	body.vx = 0.0f;
	body.vy = 0.0f;

	// Nice
	body.id = 69;

	moveType = VELOCITY;

	isDead = false;
	onGround = true;

	ballIdentification = 0;

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
	
	if (ballIdentification >= 7) {
		App->physics->winLose = false;
	}
	if (App->physics->winLose) {
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			moveType = POSITION;
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

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && onGround)
		{
			body.vy += 700 * dt;
			onGround = false;
		}

		switch (moveType)
		{
		case NONE:
			break;
		case POSITION:
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				body.x -= 20 * dt;
			}
			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				body.x += 20 * dt;
			}
			break;
		case FORCES:
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				if (body.vx >= -15)
				{
					body.AddForce(-500, 0);
				}
			}
			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				if (body.vx <= 15)
				{
					body.AddForce(500, 0);
				}
			}
			break;
		case VELOCITY:
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				if (body.vx >= -15)
				{
					body.vx = -500 * dt;
				}
			}
			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
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

		// Ball shot
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			angle_shot -= 0.2 * dt;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			angle_shot += 0.2 * dt;
		}

		if (angle_shot > 1)
		{
			angle_shot = 1;
		}
		else if (angle_shot < -1)
		{
			angle_shot = -1;
		}

		objective_x = angle_shot;
		objective_y = sqrt(1 - pow(angle_shot, 2));

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		{
			if (strength >= 80)
			{
				strength = 80;
			}
			else
			{
				strength += 0.8;
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
		{
			App->physics->CreateBall(0.5f, body.x, body.y, objective_x * strength, -objective_y * strength, ballIdentification);
			ballIdentification++;
			strength = 0;
		}

		printf("Angle Shot: %f \n", angle_shot);
		printf("Strenght: %f \n", strength);

		double px = METERS_TO_PIXELS(body.x);
		double py = SCREEN_HEIGHT - METERS_TO_PIXELS(body.y);

		printf("PX: %f, PY: %f\n", px, py);

		// SHOT DIRECTION LINE DRAWING STARTS HERE -------------------------------------------------------------
		if (strength < 3)
		{
			App->renderer->DrawLine(px, py, px + objective_x * 30, py - objective_y * 30, 0, 250, 150);
		}
		else if (strength >= 80)
		{
			// Max lenght Draw
			App->renderer->DrawLine(px, py, px + objective_x * 150, py - objective_y * 150, 255, 0, 0);
		}
		else
		{
			// Initial Draw
			App->renderer->DrawLine(px, py, px + objective_x * strength * 8, py - objective_y * strength * 8, 0, 255, 0);
			printf("AAAAAAAAAAAAAAAAAAAAAAAA %f, %f", px + objective_x * strength * 8, py - objective_y * strength * 8);
		}
	}
	else {
	if (score == 30) {
		App->fonts->BlitText(451, 239, App->fonts->textFont, "YOU WIN");
		App->fonts->BlitText(330, 265, App->fonts->textFont, "[PRESS F1 TO PLAY AGAIN]");
	}
	else {
		App->fonts->BlitText(451, 239, App->fonts->textFont, "YOU LOSE");
		App->fonts->BlitText(330, 265, App->fonts->textFont, "[PRESS F1 TO PLAY AGAIN]");
	}
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN){
		App->physics->winLose = true;
		body.mass = 10.0f; // [kg]
		body.surface = 1.0f; // [m^2]
		body.radius = 0.5f; // [m]
		body.cd = 0.4f; // [-]
		body.cl = 1.2f; // [-]
		body.b = 10.0f; // [...]
		body.coef_friction = 0.7f; // [-]
		body.coef_restitution = 0.0f; // [-]

		// Set initial position and velocity of the ball
		body.x = 25.0f;
		body.y = 10.0f;
		body.vx = 0.0f;
		body.vy = 0.0f;
		for (float i = App->scene_intro->balls.size(); i > 1 ; i--) { //HAURIA DE SER 0 QUAN ES TREGUI LA BOLA INICIAL
			App->scene_intro->balls.pop_back();
		}
		for (auto& sensor : App->scene_intro->sensorWalls)
		{
			if (!sensor.sbool) sensor.sbool = true;
		}
		ballIdentification = 0;
		score = 0;

	}
}

	return UPDATE_CONTINUE;
}