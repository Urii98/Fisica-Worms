#include "App.h"
#include "Physics.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Window.h"
#include "math.h"
#include "Collisions.h"
#include "Fonts.h"
#include <cmath>

Physics::Physics() : Module()
{
	debug = true;
}

// Destructor
Physics::~Physics()
{
}

bool Physics::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

bool Physics::Start()
{
	char lookupTableChars[] = { " !'#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[/]^_ abcdefghijklmnopqrstuvwxyz{|}~ çüéâäàaçêëèïîìäaéÆæôöòûù" };
	textFont = app->fonts->Load("Assets/pixel_font.png", lookupTableChars, 8);

	winc = true;

	ground = new Ground();
	ground->x = PIXELS_TO_METERS(0);
	ground->y = PIXELS_TO_METERS(500);
	ground->w = PIXELS_TO_METERS(500);
	ground->h = PIXELS_TO_METERS(250);

	ground->ground_col = app->col->AddCollider({ ground->x, ground->y, ground->w ,ground->h }, Collider::Type::GROUND, this);


	platform = new Ground();

	platform->x = PIXELS_TO_METERS(1200);
	platform->y = PIXELS_TO_METERS(500);
	platform->w = PIXELS_TO_METERS(100);
	platform->h = PIXELS_TO_METERS(250);

	platform->ground_col = app->col->AddCollider({ platform->x, platform->y, platform->w ,platform->h }, Collider::Type::GROUND, this);


	agua = new Ground();

	agua->x = PIXELS_TO_METERS(500);
	agua->y = PIXELS_TO_METERS(500);
	agua->w = PIXELS_TO_METERS(700);
	agua->h = PIXELS_TO_METERS(250);

	
	isla = new Ground();

	isla->x = PIXELS_TO_METERS(600);
	isla->y = PIXELS_TO_METERS(100);
	isla->w = PIXELS_TO_METERS(200);
	isla->h = PIXELS_TO_METERS(100);

	isla->ground_col = app->col->AddCollider({ isla->x, isla->y, isla->w ,isla->h }, Collider::Type::GROUND, this);


	win = new Ground();

	win->x = PIXELS_TO_METERS(1200);
	win->y = PIXELS_TO_METERS(400);
	win->w = PIXELS_TO_METERS(50);
	win->h = PIXELS_TO_METERS(50);


	integer = 1;
	movement = 1;

	return true;
}


bool Physics::PreUpdate()
{
	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		integer = 1;
	}
	else if (app->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		integer = 2;
	}
	else if (app->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		integer = 3;
	}

	if (app->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		movement = 1;
	}
	else if (app->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		movement = 2;
	}
	else if (app->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
	{
		movement = 3;
	}
	else if (app->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
	{
		gravityEnabled = !gravityEnabled;
	}
	else if (app->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	{
		buoyancyEnabled = !buoyancyEnabled;
	}
	else if (app->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
	{
		hidroDragEnabled = !hidroDragEnabled;
	}
	else if (app->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	{
		aeroDragEnabled = !aeroDragEnabled;
	}
	return true;
}

bool Physics::Update(float dt)
{
	for (size_t i = 0; i < balls.Count(); i++)
	{
		// Step #0: Reset total acceleration and total accumulated force of the ball (clear old values)
		balls.At(i)->fx = balls.At(i)->fy = 0.0;
		balls.At(i)->ax = balls.At(i)->ay = 0.0;

		// Step #1: Compute forces

		// Compute Gravity force
		double fgx = balls.At(i)->mass * 0.0;
		double fgy;
		fgy = balls.At(i)->mass * GRAVITY; // Let's assume gravity is constant and downwards
	
		//Buoyancy
		double fbuoiancy = GRAVITY * ground->density * balls.At(i)->volume;
		double fbu = -fbuoiancy;

		//Hidrodynamic Drag
		double H = 20; //variable a cambiar
		double Vfh = sqrt(pow(balls.At(i)->vx, 2) + pow(balls.At(i)->vy, 2));

		double Dfhx = -(balls.At(i)->vx / Vfh);
		double Dfhy = -(balls.At(i)->vy / Vfh);

		double Fhdx = Dfhx * H;
		double Fhdy = Dfhy * H;

		//Aerodynamic Drag
		double Fdx = balls.At(i)->vx * balls.At(i)->vx * balls.At(i)->surface * balls.At(i)->cd;
		double Fdy = balls.At(i)->vy * balls.At(i)->vy * balls.At(i)->surface * balls.At(i)->cd;

		// Add forces to the total accumulated force of the ball
		if (balls.At(i)->physics_enabled)
		{
			if (aeroDragEnabled) 
			{
				balls.At(i)->fx += Fdx;     //Aerodynamic Drag
				balls.At(i)->fy += Fdy;
			}
			if (gravityEnabled)
			{
				balls.At(i)->fy += fgy;     //Gravity
				balls.At(i)->fx += fgx;
			}
			if (balls.At(i)->y >= ground->y - balls.At(i)->rad && balls.At(i)->x >agua->x && balls.At(i)->x < platform->x)
			{
				if(buoyancyEnabled)         //Buoyancy
				{
					balls.At(i)->fy += fbu;
				}
				if (hidroDragEnabled)
				{
					balls.At(i)->fy += Fhdy;   //Hidrodynamic Drag	
					balls.At(i)->fx += Fhdx;
				}
			}
		}

		//Movimiento bola
		balls.At(i)->fx += balls.At(i)->mfx;
		balls.At(i)->fy += balls.At(i)->mfy;
		balls.At(i)->mfx = 0;
		balls.At(i)->mfy = 0;

		// Step #2: 2nd Newton's Law: SUM_Forces = mass * accel --> accel = SUM_Forces / mass
		balls.At(i)->ax = balls.At(i)->fx / balls.At(i)->mass;
		balls.At(i)->ay = balls.At(i)->fy / balls.At(i)->mass;

		// Step #3: Integrate --> from accel to new velocity & new position. 
		// We will use the 2nd order "Velocity Verlet" method for integration.
		float new_dt = dt / 1000;

		switch (integer)
		{
		case 1:
			Integrator_velocity_verlet(balls.At(i), new_dt);
			app->fonts->BlitText(0, 0, textFont, "|(1, 2, 3) Integrator: Verlet|");
			break;
		case 2:
			Integrator_forward_euler(balls.At(i), new_dt);
			app->fonts->BlitText(0, 0, textFont, "|(1, 2, 3) Integrator: Forward Euler|");
			break;
		case 3:
			Integrator_backwards_euler(balls.At(i), new_dt);
			app->fonts->BlitText(0, 0, textFont, "|(1, 2, 3) Integrator: Backwards Euler|");
			break;
		default:
			break;
		}

		switch (movement)
		{
		case 1:
			app->fonts->BlitText(0, 0, textFont, "||(4, 5, 6) Movement: Velocity");
			break;
		case 2:
			app->fonts->BlitText(0, 0, textFont, "||(4, 5, 6) Movement: Force");
			break;
		case 3:
			app->fonts->BlitText(0, 0, textFont, "||(4, 5, 6) Movement: Position");
			break;
		default:
			break;
		}

		if (gravityEnabled) {
			app->fonts->BlitText(0, 0, textFont, "|||(7) Gravity: Enabled");
		}
		else {
			app->fonts->BlitText(0, 0, textFont, "|||(7) Gravity: Disabled");
		}

		if (buoyancyEnabled) {
			app->fonts->BlitText(0, 0, textFont, "||||(8) Buoyancy: Enabled");
		}
		else {
			app->fonts->BlitText(0, 0, textFont, "||||(8) Buoyancy: Disabled");
		}

		if (hidroDragEnabled) {
			app->fonts->BlitText(0, 0, textFont, "|||||(9) Hidrodynamic Drag: Enabled");
		}
		else {
			app->fonts->BlitText(0, 0, textFont, "|||||(9) Hidrodynamic Drag: Disabled");
		}

		if (aeroDragEnabled) {
			app->fonts->BlitText(0, 0, textFont, "||||||(0) Aerodynamic Drag: Enabled");
		}
		else {
			app->fonts->BlitText(0, 0, textFont, "||||||(0) Aerodynamic Drag: Disabled");
		}


		if (balls.At(i)->ball_col != nullptr) {
			balls.At(i)->ball_col->SetPos(balls.At(i)->x - balls.At(i)->rad, balls.At(i)->y - balls.At(i)->rad);
		}
		onCol = true;
		
		
		// Step #4: solve collisions

		//Win Condition
		if (balls.At(i)->x >= win->x && balls.At(i)->x <= win->x + win->w && balls.At(i)->y >= win->y && balls.At(i)->y <= win->y + win->h)
		{
			
			winc = false;
		}

		
		//Left Wall
		if (balls.At(i)->x <= 0)
		{
			balls.At(i)->x = 0 + balls.At(i)->rad;
			balls.At(i)->vx = -balls.At(i)->vx * 0.5;
		}

		//Right Screen Wall
		if (balls.At(i)->x >= PIXELS_TO_METERS(1280))
		{
			balls.At(i)->x = PIXELS_TO_METERS(1280) - balls.At(i)->rad;
			balls.At(i)->vx = -balls.At(i)->vx * 0.5;
		}

		//Top Screen Wall
		if (balls.At(i)->y <= 0)
		{
			balls.At(i)->y = 0 + balls.At(i)->rad;
			balls.At(i)->vy = -balls.At(i)->vy * 0.5;
		}

		//Bottom Screen Wall
		if (balls.At(i)->y >= PIXELS_TO_METERS(720))
		{
			balls.At(i)->vy = balls.At(i)->vy* 0.7;
		}
	}	

	return true;
}

void Physics::OnCollision(Collider* c2, Collider* c1) {


	if (onCol == true) {


		if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::GROUND) {

			for (size_t i = 0; i < balls.Count(); i++)
			{
				if (c1 == balls.At(i)->ball_col) {
					//Particle bounce coefficient / restoration factor[0 - 1]
					float bcy = 0.5f;
					float bcx = 0.5f;
					//Space at the edges in which the collision has no effect
					float border = 1.0f;

					//Detect collisions
					//Left wall
					//if(wall is at the left of the ball)&&(ball is between the edges of the wall collider)
					if (c1->rect.x < c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w > c2->rect.x + c2->rect.w && c1->rect.y > c2->rect.y && c1->rect.y + c1->rect.h < c2->rect.y + c2->rect.h) {
						if (balls.At(i)->vx > -0.1f && balls.At(i)->vx < 0.1f) {

							balls.At(i)->x = c2->rect.x + c2->rect.w + balls.At(i)->rad;
							balls.At(i)->vx = 0;
							//balls.At(i)->gravity_enabled = false;
						}
						else {


							balls.At(i)->x = (c2->rect.x + c2->rect.w) + balls.At(i)->rad;
							balls.At(i)->vx = -bcx * balls.At(i)->vx;
							balls.At(i)->vy = bcy * balls.At(i)->vy;
						}
					}

					//Right wall
					//if (wall is at the right of the ball) && (ball is between the edges of the wall collider)
					if (c1->rect.x + c1->rect.w > c2->rect.x && c1->rect.x < c2->rect.x && c1->rect.y > c2->rect.y && c1->rect.y + c1->rect.h < c2->rect.y + c2->rect.h) {
						if (balls.At(i)->vx > -0.1f && balls.At(i)->vx < 0.1f) {

							balls.At(i)->x = c2->rect.x - balls.At(i)->rad;
							balls.At(i)->vx = 0;
							//balls.At(i)->gravity_enabled = false;
						}
						else {
							balls.At(i)->x = c2->rect.x - balls.At(i)->rad;
							balls.At(i)->vx = -bcx * balls.At(i)->vx;
							balls.At(i)->vy = bcy * balls.At(i)->vy;
						}

					}

					//Bottom wall
					// if (ball is over the wall) &&  (ball is between the edges of the wall collider)
					if (c1->rect.y + c1->rect.h > c2->rect.y && c1->rect.y < c2->rect.y && c1->rect.x > c2->rect.x - c1->rect.w && c1->rect.x + c1->rect.w < c2->rect.x + c2->rect.w) {
						if (balls.At(i)->vy > -0.1f && balls.At(i)->vy < 0.1f) {

							balls.At(i)->y = c2->rect.y - balls.At(i)->rad;
							balls.At(i)->vy = 0;
							//	balls.At(i)->gravity_enabled = false;
						}
						else {
							balls.At(i)->y = c2->rect.y - balls.At(i)->rad;
							balls.At(i)->vy = -bcy * balls.At(i)->vy;
							balls.At(i)->vx = bcx * balls.At(i)->vx;

						}

					}

					//Top wall
					// if (ball is under the wall) &&  (ball is between the edges of the wall collider)
					if (c1->rect.y < c2->rect.y + c2->rect.h && c1->rect.y + c1->rect.h >c2->rect.y+c2->rect.h && c1->rect.x > c2->rect.x - c1->rect.w && c1->rect.x + c1->rect.w < c2->rect.x + c2->rect.w) {
						
						if (balls.At(i)->vy > -0.1f && balls.At(i)->vy < 0.1f) {

							balls.At(i)->y = c2->rect.y + c2->rect.h + balls.At(i)->rad;
							balls.At(i)->vy = 0;
							//	balls.At(i)->gravity_enabled = false;
						}
						else {
							balls.At(i)->y = c2->rect.y + c2->rect.h + balls.At(i)->rad;
							balls.At(i)->vy = -bcy * balls.At(i)->vy;
							balls.At(i)->vx = bcx * balls.At(i)->vx;
						}
						
					}
				}
			}


		}

		if (c1->type == Collider::Type::BALL && c2->type == Collider::Type::GROUND) {

			for (size_t i = 0; i < balls.Count(); i++)
			{
				if (c1 == balls.At(i)->ball_col) {
					//Particle bounce coefficient / restoration factor[0 - 1]
					float bcy = 0.5f;
					float bcx = 0.5f;
					//Space at the edges in which the collision has no effect
					float border = 1.0f;

					//Detect collisions
					//Left wall
					//if(wall is at the left of the ball)&&(ball is between the edges of the wall collider)
					if (c1->rect.x < c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w > c2->rect.x + c2->rect.w && c1->rect.y > c2->rect.y && c1->rect.y + c1->rect.h < c2->rect.y + c2->rect.h) {
						if (balls.At(i)->vx > -0.1f && balls.At(i)->vx < 0.1f) {

							balls.At(i)->x = c2->rect.x + c2->rect.w + balls.At(i)->rad;
							balls.At(i)->vx = 0;
							//balls.At(i)->gravity_enabled = false;
						}
						else {


							balls.At(i)->x = (c2->rect.x + c2->rect.w) + balls.At(i)->rad;
							balls.At(i)->vx = -bcx * balls.At(i)->vx;
							balls.At(i)->vy = bcy * balls.At(i)->vy;
						}
					}

					//Right wall
					//if (wall is at the right of the ball) && (ball is between the edges of the wall collider)
					if (c1->rect.x + c1->rect.w > c2->rect.x && c1->rect.x < c2->rect.x && c1->rect.y > c2->rect.y && c1->rect.y + c1->rect.h < c2->rect.y + c2->rect.h) {
						if (balls.At(i)->vx > -0.1f && balls.At(i)->vx < 0.1f) {

							balls.At(i)->x = c2->rect.x - balls.At(i)->rad;
							balls.At(i)->vx = 0;
							//balls.At(i)->gravity_enabled = false;
						}
						else {
							balls.At(i)->x = c2->rect.x - balls.At(i)->rad;
							balls.At(i)->vx = -bcx * balls.At(i)->vx;
							balls.At(i)->vy = bcy * balls.At(i)->vy;
						}

					}

					//Bottom wall
					// if (ball is over the wall) &&  (ball is between the edges of the wall collider)
					if (c1->rect.y + c1->rect.h > c2->rect.y && c1->rect.y < c2->rect.y && c1->rect.x > c2->rect.x - c1->rect.w && c1->rect.x + c1->rect.w < c2->rect.x + c2->rect.w) {
						if (balls.At(i)->vy > -0.1f && balls.At(i)->vy < 0.1f) {

							balls.At(i)->y = c2->rect.y - balls.At(i)->rad;
							balls.At(i)->vy = 0;
							//	balls.At(i)->gravity_enabled = false;
						}
						else {
							balls.At(i)->y = c2->rect.y - balls.At(i)->rad;
							balls.At(i)->vy = -bcy * balls.At(i)->vy;
							balls.At(i)->vx = bcx * balls.At(i)->vx;

						}

					}

					//Top wall
					// if (ball is under the wall) &&  (ball is between the edges of the wall collider)
					if (c1->rect.y < c2->rect.y + c2->rect.h && c1->rect.y + c1->rect.h >c2->rect.y + c2->rect.h && c1->rect.x > c2->rect.x - c1->rect.w && c1->rect.x + c1->rect.w < c2->rect.x + c2->rect.w) {

						if (balls.At(i)->vy > -0.1f && balls.At(i)->vy < 0.1f) {

							balls.At(i)->y = c2->rect.y + c2->rect.h + balls.At(i)->rad;
							balls.At(i)->vy = 0;
							//	balls.At(i)->gravity_enabled = false;
						}
						else {
							balls.At(i)->y = c2->rect.y + c2->rect.h + balls.At(i)->rad;
							balls.At(i)->vy = -bcy * balls.At(i)->vy;
							balls.At(i)->vx = bcx * balls.At(i)->vx;
						}

					}
				}
			}
		}
		//onCol = false;
	}
}


bool Physics::PostUpdate()
{

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (!debug)
		return true;

	for (size_t i = 1; i < balls.Count(); i++) // is player
	{
		app->render->DrawCircle(METERS_TO_PIXELS(balls.At(i)->x), METERS_TO_PIXELS(balls.At(i)->y), METERS_TO_PIXELS(balls.At(i)->rad), 255, 0, 0);
	}
	
	//Suelo
	app->render->DrawRectangle({ METERS_TO_PIXELS(ground->x), METERS_TO_PIXELS(ground->y), METERS_TO_PIXELS(ground->w), METERS_TO_PIXELS(ground->h) }, 0, 255, 0, 255, false);

	//Plataforma
	app->render->DrawRectangle({ METERS_TO_PIXELS(platform->x), METERS_TO_PIXELS(platform->y), METERS_TO_PIXELS(platform->w), METERS_TO_PIXELS(platform->h) }, 0, 255, 0, 255, false);

	//Agua
	app->render->DrawRectangle({ METERS_TO_PIXELS(agua->x), METERS_TO_PIXELS(agua->y), METERS_TO_PIXELS(agua->w), METERS_TO_PIXELS(agua->h) }, 0, 47, 187, 255, false);

	//Isla
	app->render->DrawRectangle({ METERS_TO_PIXELS(isla->x), METERS_TO_PIXELS(isla->y), METERS_TO_PIXELS(isla->w), METERS_TO_PIXELS(isla->h) }, 0, 143, 57, 255, false);

	//Win
	app->render->DrawRectangle({ METERS_TO_PIXELS(win->x), METERS_TO_PIXELS(win->y), METERS_TO_PIXELS(win->w), METERS_TO_PIXELS(win->h) }, 0, 255, 0, 255, false);

	return true;
}

void Physics::Integrator_velocity_verlet(Ball* ball, double dt)
{
	ball->x += ball->vx * dt + 0.5 * ball->ax * dt * dt;
	ball->y += ball->vy * dt + 0.5 * ball->ay * dt * dt;

	ball->vx += ball->ax * dt;
	ball->vy += ball->ay * dt;
}

void Physics::Integrator_forward_euler(Ball* ball, double dt)
{
	ball->vx = ball->vx + ball->ax * dt;
	ball->vy = ball->vy + ball->ay * dt;

	ball->x = ball->x + ball->vx * dt;
	ball->y = ball->y + ball->vy * dt;

}

void Physics::Integrator_backwards_euler(Ball* ball, double dt)
{
	ball->x = ball->x + ball->vx * dt;
	ball->y = ball->y + ball->vy * dt;

	ball->vx = ball->vx + ball->ax * dt;
	ball->vy = ball->vy + ball->ay * dt;

}


int Physics::CreateBall(double mass, double rad, double x, double y, double vx, double vy)
{
	Ball* new_ball = new Ball();

	// Set physics properties of the ball
	new_ball->mass = mass; // kg
	new_ball->rad = PIXELS_TO_METERS(rad);
	new_ball->surface = new_ball->rad * M_PI;
	new_ball->volume = pow(new_ball->rad,3) * M_PI * 1.3333;

	// Set initial position and velocity of the ball
	new_ball->x = PIXELS_TO_METERS(x);
	new_ball->y = PIXELS_TO_METERS(y);
	new_ball->vx = vx;
	new_ball->vy = vy;

	//Add collider
	new_ball->ball_col = app->col->AddCollider({ new_ball->x - new_ball->rad, new_ball->y - new_ball->rad, new_ball->rad * 2 , new_ball->rad * 2 }, Collider::Type::BALL, this);

	new_ball->cd = 0.4;

	balls.Insert(*new_ball, balls.Count()); 

	return balls.Count() - 1;
}


void Ball::SetVelocity(double velX, double velY)
{
	vx = velX;
	vy = velY;
}

void Ball::AddForce(double forX, double forY)
{
	mfx = forX;
	mfy = forY;
}

void Ball::SetPosition(double X, double Y) 
{
	x += PIXELS_TO_METERS(X);
	y += PIXELS_TO_METERS(Y);
}

dPoint Ball::GetVelocity()
{
	dPoint velocity = { vx, vy };

	return velocity;
}


// Called before quitting
bool Physics::CleanUp()
{
	LOG("Destroying physics world");

	return true;
}
