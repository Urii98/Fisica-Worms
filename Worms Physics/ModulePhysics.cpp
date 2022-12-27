#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"
#include <cmath>

// TODO 1: Include Box 2 header and library

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	gravity = -10.0f;
	buoyancy = 0.0f;
	aeroDragX, aeroDragY = 0.0f;
	hidroDragX, hidroDragY = 0.0f;

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	// Process all balls in the scenario
	for (auto& ball : App->scene_intro->balls)
	{
		// Skip ball if physics not enabled
		if (!ball.physics_enabled)
		{
			continue;
		}

		// Step #0: Clear old values
		// ----------------------------------------------------------------------------------------
		
		// Reset total acceleration and total accumulated force of the ball
		ball.fx = ball.fy = 0.0f;
		ball.ax = ball.ay = 0.0f;

		// Step #1: Compute forces
		// ----------------------------------------------------------------------------------------

		// Gravity force
		float fgx = ball.mass * 0.0f;
		float fgy = ball.mass * gravity; // Let's assume gravity is constant and downwards
		ball.fx += fgx; ball.fy += fgy; // Add this force to ball's total force

		// Aerodynamic Drag force (only when not in water)
		if (!is_colliding_with_water(ball, App->scene_intro->water1))
		{
			aeroDragX = 0.0f; aeroDragY = 0.0f;
			compute_aerodynamic_drag(aeroDragX, aeroDragY, ball, App->scene_intro->atmosphere);
			ball.fx += aeroDragX; ball.fy += aeroDragY; // Add this force to ball's total force
		}

		// Hydrodynamic forces (only when in water)
		if (is_colliding_with_water(ball, App->scene_intro->water1))
		{
			// Hydrodynamic Drag force
			hidroDragX = 0.0f; hidroDragY = 0.0f;
			compute_hydrodynamic_drag(hidroDragX, hidroDragY, ball, App->scene_intro->water1);
			ball.fx += hidroDragX; ball.fy += hidroDragY; // Add this force to ball's total force

			// Hydrodynamic Buoyancy force
			float fhbx = 0.0f; buoyancy = 0.0f;
			compute_hydrodynamic_buoyancy(fhbx, buoyancy, ball, App->scene_intro->water1);
			ball.fx += fhbx; ball.fy += buoyancy; // Add this force to ball's total force
		}

		// Other forces
		// ...

		// Step #2: 2nd Newton's Law
		// ----------------------------------------------------------------------------------------
		
		// SUM_Forces = mass * accel --> accel = SUM_Forces / mass
		ball.ax = ball.fx / ball.mass;
		ball.ay = ball.fy / ball.mass;

		// Step #3: Integrate --> from accel to new velocity & new position
		// ----------------------------------------------------------------------------------------

		// We will use the 2nd order "Velocity Verlet" method for integration.
		integrator_velocity_verlet(ball, dt);
		//integrator_backwards_euler(ball, dt);
		//integrator_forward_euler(ball, dt);

		// Step #4: solve collisions
		// ----------------------------------------------------------------------------------------

		// Solve collision between ball and ground
		if (is_colliding_with_ground(ball, App->scene_intro->ground))
		{
			// TP ball to ground surface
			ball.y = App->scene_intro->ground.y + App->scene_intro->ground.h + ball.radius;

			// Elastic bounce with ground
			ball.vy = - ball.vy;

			// FUYM non-elasticity
			ball.vx *= ball.coef_friction;
			ball.vy *= ball.coef_restitution;
		}

		// Solve collision between ball and wall (and window borders)
		for (auto& wall : App->scene_intro->walls)
		{
			if (is_colliding_with_wall(ball, wall))
			{
				// Ball is on left
				// if (wall is at the right of the ball) && (ball is between the edges of the wall collider)
				if (ball.x + ball.radius > wall.x && ball.x < wall.x && ball.y > wall.y
					&& ball.y + ball.radius < wall.y + wall.h)
				{
					// Elastic bounce with wall
					ball.vx = -ball.vx * wall.bouncyness;
					ball.vy = ball.vy * wall.bouncyness;
				}
				//Right wall
				//if (wall is at the right of the ball) && (ball is between the edges of the wall collider)
				else if (ball.x + ball.radius > wall.x && ball.x < wall.x &&
					ball.y > App->scene_intro->wall2.y && ball.y + ball.radius < wall.y + wall.h)
				{
					// Elastic bounce with wall
					ball.vx = -ball.vx * wall.bouncyness;
					ball.vy = ball.vy * wall.bouncyness;
				}
				//Bottom wall
				// if (ball is over the wall) &&  (ball is between the edges of the wall collider)
				else if (ball.y + ball.radius > wall.y && ball.y < wall.y &&
					ball.x > wall.x - ball.radius && ball.x + ball.radius < wall.x + wall.w)
				{
					// Elastic bounce with wall
					ball.vx = ball.vx * wall.bouncyness;
					ball.vy = -ball.vy * wall.bouncyness;
				}
				//Top wall
				// if (ball is under the wall) &&  (ball is between the edges of the wall collider)
				else if (ball.y < wall.y + wall.h && ball.y + ball.radius >wall.y + wall.h &&
					ball.x > wall.x - ball.radius && ball.x + ball.radius < wall.x + wall.w)
				{
					// Elastic bounce with wall
					ball.vx = ball.vx * wall.bouncyness;
					ball.vy = -ball.vy * wall.bouncyness;
				}

				// FUYM non-elasticity
				ball.vx *= ball.coef_friction;
				ball.vy *= ball.coef_restitution;
			}

			// Left Border
			if (ball.x + ball.radius <= 0)
			{
				ball.x = 0 + ball.radius;
				ball.vx = -ball.vx * 0.5f;
			}
			// Right Border
			if (ball.x >= PIXEL_TO_METERS(SCREEN_WIDTH))
			{
				ball.x = PIXEL_TO_METERS(SCREEN_WIDTH) - ball.radius;
				ball.vx = -ball.vx * 0.5f;
			}
			// Top Border
			if (ball.y <= 0)
			{
				ball.y = 0 + ball.radius;
				ball.vy = -ball.vy * 0.5f;
			}
			// Bottom Border
			if (ball.y >= PIXEL_TO_METERS(SCREEN_HEIGHT))
			{
				ball.vy = -ball.vy * 0.5f;
			}
		}
		
	}

	// Continue game
	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
	// Colors
	int color_r, color_g, color_b;

	// Draw ground
	color_r = 0; color_g = 255; color_b = 0;
	App->renderer->DrawQuad(App->scene_intro->ground.pixels(), color_r, color_g, color_b);

	// Draw walls
	color_r = 0; color_g = 255; color_b = 0;
	App->renderer->DrawQuad(App->scene_intro->wall1.pixels(), color_r, color_g, color_b);
	App->renderer->DrawQuad(App->scene_intro->wall2.pixels(), color_r, color_g, color_b);

	color_r = 255; color_g = 155; color_b = 0;
	App->renderer->DrawQuad(App->scene_intro->wall3.pixels(), color_r, color_g, color_b);
	App->renderer->DrawQuad(App->scene_intro->wall4.pixels(), color_r, color_g, color_b);
	App->renderer->DrawQuad(App->scene_intro->wall5.pixels(), color_r, color_g, color_b);

	App->renderer->DrawQuad(App->scene_intro->wall6.pixels(), color_r, color_g, color_b);
	App->renderer->DrawQuad(App->scene_intro->wall7.pixels(), color_r, color_g, color_b);
	App->renderer->DrawQuad(App->scene_intro->wall8.pixels(), color_r, color_g, color_b);

	App->renderer->DrawQuad(App->scene_intro->wall9.pixels(), color_r, color_g, color_b);
	App->renderer->DrawQuad(App->scene_intro->wall10.pixels(), color_r, color_g, color_b);
	App->renderer->DrawQuad(App->scene_intro->wall11.pixels(), color_r, color_g, color_b);

	// Draw bouncy walls
	color_r = 50; color_g = 100; color_b = 50;
	App->renderer->DrawQuad(App->scene_intro->wall12.pixels(), color_r, color_g, color_b);
	App->renderer->DrawQuad(App->scene_intro->wall13.pixels(), color_r, color_g, color_b);

	// Draw water1
	color_r = 0; color_g = 0; color_b = 255;
	App->renderer->DrawQuad(App->scene_intro->water1.pixels(), color_r, color_g, color_b);

	// Draw water2
	color_r = 0; color_g = 0; color_b = 255;
	App->renderer->DrawQuad(App->scene_intro->water2.pixels(), color_r, color_g, color_b);

	// Draw all balls in the scenario
	for (auto& ball : App->scene_intro->balls)
	{
		// Convert from physical magnitudes to geometrical pixels
		int pos_x = METERS_TO_PIXELS(ball.x);
		int pos_y = SCREEN_HEIGHT - METERS_TO_PIXELS(ball.y);
		int size_r = METERS_TO_PIXELS(ball.radius);

		// Select color
		if (ball.physics_enabled)
		{
			color_r = 255; color_g = 255; color_b = 255;
		}
		else
		{
			color_r = 255; color_g = 0; color_b = 0;
		}

		// Draw ball
		App->renderer->DrawCircle(pos_x, pos_y, size_r, color_r, color_g, color_b);
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	return true;
}

// Compute modulus of a vector
float modulus(float vx, float vy)
{
	return std::sqrt(vx * vx + vy * vy);
}

// Compute Aerodynamic Drag force
void compute_aerodynamic_drag(float &fx, float& fy, const PhysBall &ball, const Atmosphere &atmosphere)
{
	float rel_vel[2] = { ball.vx - atmosphere.windx, ball.vy - atmosphere.windy }; // Relative velocity
	float speed = modulus(rel_vel[0], rel_vel[1]); // Modulus of the relative velocity
	float rel_vel_unitary[2] = { rel_vel[0] / speed, rel_vel[1] / speed }; // Unitary vector of relative velocity
	float fdrag_modulus = 0.5f * atmosphere.density * speed * speed * ball.surface * ball.cd; // Drag force (modulus)
	fx = -rel_vel_unitary[0] * fdrag_modulus; // Drag is antiparallel to relative velocity
	fy = -rel_vel_unitary[1] * fdrag_modulus; // Drag is antiparallel to relative velocity
}

// Compute Hydrodynamic Drag force
void compute_hydrodynamic_drag(float& fx, float& fy, const PhysBall& ball, const Water& water)
{
	float rel_vel[2] = { ball.vx - water.vx, ball.vy - water.vy }; // Relative velocity
	float speed = modulus(rel_vel[0], rel_vel[1]); // Modulus of the relative velocity
	float rel_vel_unitary[2] = { rel_vel[0] / speed, rel_vel[1] / speed }; // Unitary vector of relative velocity
	float fdrag_modulus = ball.b * speed; // Drag force (modulus)
	fx = -rel_vel_unitary[0] * fdrag_modulus; // Drag is antiparallel to relative velocity
	fy = -rel_vel_unitary[1] * fdrag_modulus; // Drag is antiparallel to relative velocity
}

// Compute Hydrodynamic Buoyancy force
void compute_hydrodynamic_buoyancy(float& fx, float& fy, const PhysBall& ball, const Water& water)
{
	// Compute submerged area (assume ball is a rectangle, for simplicity)
	float water_top_level = water.y + water.h; // Water top level y
	float h = 2.0f * ball.radius; // Ball "hitbox" height
	float surf = h * (water_top_level - ball.y); // Submerged surface
	if ((ball.y + ball.radius) < water_top_level) surf = h * h; // If ball completely submerged, use just all ball area
	surf *= 0.4; // FUYM to adjust values (should compute the area of circle segment correctly instead; I'm too lazy for that)

	// Compute Buoyancy force
	double fbuoyancy_modulus = water.density * 10.0 * surf; // Buoyancy force (modulus)
	fx = 0.0; // Buoyancy is parallel to pressure gradient
	fy = fbuoyancy_modulus; // Buoyancy is parallel to pressure gradient
}

// Integration scheme: Velocity Verlet
void integrator_velocity_verlet(PhysBall& ball, float dt)
{
	ball.x += ball.vx * dt + 0.5f * ball.ax * dt * dt;
	ball.y += ball.vy * dt + 0.5f * ball.ay * dt * dt;
	ball.vx += ball.ax * dt;
	ball.vy += ball.ay * dt;
}

void integrator_forward_euler(PhysBall& ball, double dt)
{
	ball.vx = ball.vx + ball.ax * dt;
	ball.vy = ball.vy + ball.ay * dt;

	ball.x = ball.x + ball.vx * dt;
	ball.y = ball.y + ball.vy * dt;
}

void integrator_backwards_euler(PhysBall& ball, double dt)
{
	ball.x = ball.x + ball.vx * dt;
	ball.y = ball.y + ball.vy * dt;

	ball.vx = ball.vx + ball.ax * dt;
	ball.vy = ball.vy + ball.ay * dt;
}


// Detect collision with ground
bool is_colliding_with_ground(const PhysBall& ball, const Ground& ground)
{
	float rect_x = (ground.x + ground.w / 2.0f); // Center of rectangle
	float rect_y = (ground.y + ground.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, ground.w, ground.h);
}

// Detect collision with wall
bool is_colliding_with_wall(const PhysBall& ball, const Wall& wall)
{
	float rect_x = (wall.x + wall.w / 2.0f); // Center of rectangle
	float rect_y = (wall.y + wall.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, wall.w, wall.h);
}

// Detect collision with water
bool is_colliding_with_water(const PhysBall& ball, const Water& water)
{
	float rect_x = (water.x + water.w / 2.0f); // Center of rectangle
	float rect_y = (water.y + water.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, water.w, water.h);
}

// Detect collision between circle and rectange
bool check_collision_circle_rectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh)
{
	// Algorithm taken from https://stackoverflow.com/a/402010

	// Distance from center of circle to center of rectangle
	float dist_x = std::abs(cx - rx);
	float dist_y = std::abs(cy - ry);

	// If circle is further than half-rectangle, not intersecting
	if (dist_x > (rw / 2.0f + cr)) { return false; }
	if (dist_y > (rh / 2.0f + cr)) { return false; }

	// If circle is closer than half-rectangle, is intersecting
	if (dist_x <= (rw / 2.0f)) { return true; }
	if (dist_y <= (rh / 2.0f)) { return true; }

	// If all of above fails, check corners
	float a = dist_x - rw / 2.0f;
	float b = dist_y - rh / 2.0f;
	float cornerDistance_sq = a * a + b * b;
	return (cornerDistance_sq <= (cr * cr));
}

// Convert from meters to pixels (for SDL drawing)
SDL_Rect Ground::pixels()
{
	SDL_Rect pos_px{};
	pos_px.x = METERS_TO_PIXELS(x);
	pos_px.y = SCREEN_HEIGHT - METERS_TO_PIXELS(y);
	pos_px.w = METERS_TO_PIXELS(w);
	pos_px.h = METERS_TO_PIXELS(-h); // Can I do this? LOL
	return pos_px;
}


