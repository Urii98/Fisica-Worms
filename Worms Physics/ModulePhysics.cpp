#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"
#include "ModulePlayer.h"
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
	integrador = 0;

	buoyancyEnabled = true;
	aeroDragXEnabled = true;
	aeroDragYEnabled = true;
	hidroDragXEnabled = true;
	hidroDragYEnabled = true;

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	// Process player in the scenario
	if (App->player->body.physics_enabled)
	{
		// Step #0: Clear old values
	// ----------------------------------------------------------------------------------------

	// Reset total acceleration and total accumulated force of the App->player->body
		App->player->body.fx = App->player->body.fy = 0.0f;
		App->player->body.ax = App->player->body.ay = 0.0f;

		// Step #1: Compute forces
		// ----------------------------------------------------------------------------------------

		// Gravity force
		float fgx = App->player->body.mass * 0.0f;
		float fgy = App->player->body.mass * gravity; // Let's assume gravity is constant and downwards
		App->player->body.fx += fgx; 
		App->player->body.fy += fgy;
		// Add this force to App->player->body's total force

		// Water kills the player
		for (auto& water : App->scene_intro->waters)
		{
			// Hydrodynamic forces (only when in water)
			if (is_colliding_with_water(App->player->body, water))
			{
				App->player->isDead = true;
			}
		}

		//Movimiento bola
		App->player->body.fx += App->player->body.mfx;
		App->player->body.fy += App->player->body.mfy;
		App->player->body.mfx = 0;
		App->player->body.mfy = 0;

		// Step #2: 2nd Newton's Law
		// ----------------------------------------------------------------------------------------

		App->player->body.ax = App->player->body.fx / App->player->body.mass;
		App->player->body.ay = App->player->body.fy / App->player->body.mass;

		
		

		// Step #3: Integrate --> from accel to new velocity & new position
		// ----------------------------------------------------------------------------------------

		switch (integrador)
		{
		case 0:
			integrator_velocity_verlet(App->player->body, dt);
			break;
		case 1:
			integrator_backwards_euler(App->player->body, dt);
			break;
		case 2:
			integrator_forward_euler(App->player->body, dt);
			break;
		}
		printf("\nPLAYER ACCEL: %f, %f\n", App->player->body.ax, App->player->body.ay);

		// Step #4: solve collisions
		// ----------------------------------------------------------------------------------------

		// Solve collision between App->player->body and ground
		if (is_colliding_with_ground(App->player->body, App->scene_intro->ground))
		{
			App->player->onGround = true;
			// TP App->player->body to ground surface
			App->player->body.y = App->scene_intro->ground.y + App->scene_intro->ground.h + App->player->body.radius;

			// Elastic bounce with ground
			App->player->body.vy = -App->player->body.vy;

			// FUYM non-elasticity
			App->player->body.vx *= App->player->body.coef_friction;
			App->player->body.vy *= App->player->body.coef_restitution;
		}

		// Solve collision between App->player->body and wall (and window borders)
		for (auto& wall : App->scene_intro->walls)
		{
			if (is_colliding_with_wall(App->player->body, wall))
			{
				App->player->onGround = true;

				// App->player->body is on right
				//if(wall is at the left of the App->player->body)&&(App->player->body is between the edges of the wall collider)
				if (App->player->body.x - App->player->body.radius < wall.x + wall.w && App->player->body.x + App->player->body.radius > wall.x + wall.w &&
					App->player->body.y > wall.y && App->player->body.y + App->player->body.radius < wall.y + wall.h)
				{
					if (App->player->body.vx > -0.1f && App->player->body.vx < 0.1f)
					{
						App->player->body.x = wall.x + wall.w + App->player->body.radius;
						App->player->body.vx = 0;
					}
					else
					{
						App->player->body.x = (wall.x + wall.w) + App->player->body.radius;
						// Elastic bounce with wall
						App->player->body.vx = -App->player->body.vx * wall.bouncyness;
						App->player->body.vy = App->player->body.vy * wall.bouncyness;
					}

				}
				//App->player->body is on left
				//if (wall is at the right of the App->player->body) && (App->player->body is between the edges of the wall collider)
				if (App->player->body.x + App->player->body.radius > wall.x && App->player->body.x < wall.x &&
					App->player->body.y > wall.y && App->player->body.y + App->player->body.radius < wall.y + wall.h)
				{
					if (App->player->body.vx > -0.1f && App->player->body.vx < 0.1f)
					{
						App->player->body.x = wall.x - App->player->body.radius;
						App->player->body.vx = 0;
					}
					else
					{
						App->player->body.x = wall.x - App->player->body.radius;
						// Elastic bounce with wall
						App->player->body.vx = -App->player->body.vx * wall.bouncyness;
						App->player->body.vy = App->player->body.vy * wall.bouncyness;
					}
				}
				//App->player->body over wall
				// if (App->player->body is over the wall) &&  (App->player->body is between the edges of the wall collider)
				if (App->player->body.y + App->player->body.radius > wall.y && App->player->body.y < wall.y && App->player->body.x > wall.x - App->player->body.radius && App->player->body.x + App->player->body.radius < wall.x + wall.w)
				{
					App->player->onGround = true;
					if (App->player->body.vy > -0.1f && App->player->body.vy < 0.1f)
					{
						App->player->body.y = wall.y - App->player->body.radius;
						App->player->body.vy = 0;
					}
					else
					{
						App->player->body.y = wall.y - App->player->body.radius;
						// Elastic bounce with wall
						App->player->body.vx = App->player->body.vx * wall.bouncyness;
						App->player->body.vy = -App->player->body.vy * wall.bouncyness;
					}
				}
				// App->player->body under wall
				// if (App->player->body is under the wall) &&  (App->player->body is between the edges of the wall collider)
				if (App->player->body.y - App->player->body.radius < wall.y + wall.h && App->player->body.y + App->player->body.radius > wall.y + wall.h &&
					App->player->body.x > wall.x - App->player->body.radius && App->player->body.x + App->player->body.radius < wall.x + wall.w)
				{
					if (App->player->body.vy > -0.1f && App->player->body.vy < 0.1f)
					{
						App->player->body.y = wall.y + wall.h + App->player->body.radius;
						App->player->body.vy = 0;
					}
					else
					{
						App->player->body.y = wall.y + wall.h + App->player->body.radius;
						// Elastic bounce with wall
						App->player->body.vx = App->player->body.vx * wall.bouncyness;
						App->player->body.vy = -App->player->body.vy * wall.bouncyness;
					}

				}

				// FUYM non-elasticity
				App->player->body.vx *= App->player->body.coef_friction;
				App->player->body.vy *= App->player->body.coef_restitution;
			}

			// Left Border
			if (App->player->body.x + App->player->body.radius <= 0)
			{
				App->player->body.x = 0 + App->player->body.radius;
				App->player->body.vx = -App->player->body.vx * 0.5f;
			}
			// Right Border
			if (App->player->body.x >= PIXEL_TO_METERS(SCREEN_WIDTH))
			{
				App->player->body.x = PIXEL_TO_METERS(SCREEN_WIDTH) - App->player->body.radius;
				App->player->body.vx = -App->player->body.vx * 0.5f;
			}
			// Top Border
			if (App->player->body.y <= 0)
			{
				App->player->body.y = 0 + App->player->body.radius;
				App->player->body.vy = -App->player->body.vy * 0.5f;
			}
			// Bottom Border
			if (App->player->body.y >= PIXEL_TO_METERS(SCREEN_HEIGHT))
			{
				App->player->body.vy = -App->player->body.vy * 0.5f;
			}


			//// Hem decidit limitar la velocitat per evitar tunneling amb les colisions
			//if (App->player->body.vx > 20.0f)
			//{
			//	App->player->body.vx = 19.99f;
			//}
			//if (App->player->body.vx < -20.0f)
			//{
			//	App->player->body.vx = -19.99f;
			//}
			//if (App->player->body.vy > 20.0f)
			//{
			//	App->player->body.vy = 19.99f;
			//}
			//if (App->player->body.vy < -20.0f)
			//{
			//	App->player->body.vy = -19.99f;
			//}
			//printf("%f\n", App->player->body.vx);
		}
	}
	

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



		// Water physics
		for (auto& water : App->scene_intro->waters)
		{
			// Aerodynamic Drag force (only when not in water)
			if (!is_colliding_with_water(ball, water))
			{
				aeroDragX = 0.0f; aeroDragY = 0.0f;
				compute_aerodynamic_drag(aeroDragX, aeroDragY, ball, App->scene_intro->atmosphere);
				ball.fx += aeroDragX; ball.fy += aeroDragY; // Add this force to ball's total force
			}

			// Hydrodynamic forces (only when in water)
			if (is_colliding_with_water(ball, water))
			{
				// Hydrodynamic Drag force
				hidroDragX = 0.0f; hidroDragY = 0.0f;
				compute_hydrodynamic_drag(hidroDragX, hidroDragY, ball, water);
				ball.fx += hidroDragX; ball.fy += hidroDragY; // Add this force to ball's total force

				// Hydrodynamic Buoyancy force
				float fhbx = 0.0f; buoyancy = 0.0f;
				compute_hydrodynamic_buoyancy(fhbx, buoyancy, ball, water);
				ball.fx += fhbx; ball.fy += buoyancy; // Add this force to ball's total force
			}
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

		switch (integrador)
		{
		case 0:
			integrator_velocity_verlet(ball, dt);
			break;
		case 1:
			integrator_backwards_euler(ball, dt);
			break;
		case 2:
			integrator_forward_euler(ball, dt);
			break;
		}


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
			// Wall - ball collision solver OLD code:
			if (is_colliding_with_wall(ball, wall))
			{
				// Ball is on right
				//if(wall is at the left of the ball)&&(ball is between the edges of the wall collider)
				if (ball.x - ball.radius < wall.x + wall.w && ball.x + ball.radius > wall.x + wall.w &&
					ball.y > wall.y && ball.y + ball.radius < wall.y + wall.h)
				{
					if (ball.vx > -0.1f && ball.vx < 0.1f)
					{
						ball.x = wall.x + wall.w + ball.radius;
						ball.vx = 0;
					}
					else
					{
						ball.x = (wall.x + wall.w) + ball.radius;
						// Elastic bounce with wall
						ball.vx = -ball.vx * wall.bouncyness;
						ball.vy = ball.vy * wall.bouncyness;
					}

				}
				//Ball is on left
				//if (wall is at the right of the ball) && (ball is between the edges of the wall collider)
				if (ball.x + ball.radius > wall.x && ball.x < wall.x &&
					ball.y > wall.y && ball.y + ball.radius < wall.y + wall.h)
				{
					if (ball.vx > -0.1f && ball.vx < 0.1f)
					{
						ball.x = wall.x - ball.radius;
						ball.vx = 0;
					}
					else
					{
						ball.x = wall.x - ball.radius;
						// Elastic bounce with wall
						ball.vx = -ball.vx * wall.bouncyness;
						ball.vy = ball.vy * wall.bouncyness;
					}
				}
				// Ball above wall
				// if (ball is above the wall) && (ball is between the horizontal edges of the wall collider)
				if (ball.y - ball.radius < wall.y + wall.h && //Ball's bottom-side is below wall's top-side
					ball.y + ball.radius > wall.y + wall.h && // Ball's top-side is above wall's top-side
					ball.x + ball.radius > wall.x && //Ball's right-side is > wall's left-side
					ball.x - ball.radius < wall.x + wall.w) // Ball's left-side is < wall's right-side
				{
					ball.y = wall.y + wall.h + ball.radius;
					// Elastic bounce with wall
					ball.vx = ball.vx * wall.bouncyness;
					ball.vy = -ball.vy * wall.bouncyness;
				}
				// Ball under wall
				// if (ball is under the wall) &&  (ball is between the horizontal edges of the wall collider)
				if (ball.y - ball.radius < wall.y && //Ball's bottom-side is below wall's bottom-side
					ball.y + ball.radius > wall.y && // Ball's top-side is above wall's bottom-side
					ball.x + ball.radius > wall.x && //Ball's right-side is > wall's left-side
					ball.x - ball.radius < wall.x + wall.w) // Ball's left-side is < wall's right-side
				{
					ball.y = wall.y - ball.radius;
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


			//// Hem decidit limitar la velocitat per evitar tunneling amb les colisions
			//if (ball.vx > 20.0f)
			//{
			//	ball.vx = 19.99f;
			//}
			//if (ball.vx < -20.0f)
			//{
			//	ball.vx = -19.99f;
			//}
			//if (ball.vy > 20.0f)
			//{
			//	ball.vy = 19.99f;
			//}
			//if (ball.vy < -20.0f)
			//{
			//	ball.vy = -19.99f;
			//}
			//printf("%f\n", ball.vx);
		}
		for (auto& sensor : App->scene_intro->sensorWalls)
		{
			
			if (is_colliding_with_sensor(ball, sensor))
			{
				if (sensor.sbool) {
					App->player->score += 10;
					sensor.sbool = false;
					
				}
			}
		}		
	}

	// -------------------- ATTENTION ------------------------------------
	// ------------------- ¡¡DANGER!! ------------------------------------
	//ESTO TIENE QUE SER LO ULTIMO DEL PREUPDATE, NO PUEDE HABER NADA DESPUÉS
	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
	{
		integrador++;
		if (integrador > 2)
		{
			integrador = 0;
		}
	}
	// -------------------- ATTENTION ------------------------------------
	// ------------------- ¡¡DANGER!! ------------------------------------

	// Continue game
	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
	// Colors
	int color_r, color_g, color_b;

	//Draw sensors
	for (auto& sensor : App->scene_intro->sensorWalls)
	{
		if (!sensor.sbool) {
			color_r = 0; color_g = 255; color_b = 0;
			App->renderer->DrawQuad(sensor.pixels(), color_r, color_g, color_b);
		}
		else {
			color_r = 255; color_g = 0; color_b = 0;
			App->renderer->DrawQuad(sensor.pixels(), color_r, color_g, color_b);
		}
	}

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

	//// Draw Sensors
	//color_r = 0; color_g = 255; color_b = 255;
	//App->renderer->DrawQuad(App->scene_intro->sensor1.pixels(), color_r, color_g, color_b);
	//App->renderer->DrawQuad(App->scene_intro->sensor2.pixels(), color_r, color_g, color_b);
	//App->renderer->DrawQuad(App->scene_intro->sensor3.pixels(), color_r, color_g, color_b);

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

	// Draw Player
	// Convert from physical magnitudes to geometrical pixels
	int pos_x = METERS_TO_PIXELS(App->player->body.x);
	int pos_y = SCREEN_HEIGHT - METERS_TO_PIXELS(App->player->body.y);
	int size_r = METERS_TO_PIXELS(App->player->body.radius);

	color_r = 0; color_g = 50; color_b = 255;
	// Draw ball
	App->renderer->DrawCircle(pos_x, pos_y, size_r, color_r, color_g, color_b);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	return true;
}

// Compute modulus of a vector
float ModulePhysics::modulus(float vx, float vy)
{
	return std::sqrt(vx * vx + vy * vy);
}

// Compute Aerodynamic Drag force
void ModulePhysics::compute_aerodynamic_drag(float &fx, float& fy, const PhysBall &ball, const Atmosphere &atmosphere)
{
	if (!aeroDragXEnabled && !aeroDragYEnabled)
	{
		return;
	}

	float rel_vel[2] = { ball.vx - atmosphere.windx, ball.vy - atmosphere.windy }; // Relative velocity
	float speed = modulus(rel_vel[0], rel_vel[1]); // Modulus of the relative velocity
	float rel_vel_unitary[2] = { rel_vel[0] / speed, rel_vel[1] / speed }; // Unitary vector of relative velocity
	float fdrag_modulus = 0.5f * atmosphere.density * speed * speed * ball.surface * ball.cd; // Drag force (modulus)
	if (aeroDragXEnabled)
	{
		fx = -rel_vel_unitary[0] * fdrag_modulus; // Drag is antiparallel to relative velocity
	}
	if (aeroDragYEnabled)
	{
		fy = -rel_vel_unitary[1] * fdrag_modulus; // Drag is antiparallel to relative velocity
	}
	
}

// Compute Hydrodynamic Drag force
void ModulePhysics::compute_hydrodynamic_drag(float& fx, float& fy, const PhysBall& ball, const Water& water)
{

	if (!hidroDragXEnabled && !hidroDragYEnabled)
	{
		return;
	}

	float rel_vel[2] = { ball.vx - water.vx, ball.vy - water.vy }; // Relative velocity
	float speed = modulus(rel_vel[0], rel_vel[1]); // Modulus of the relative velocity
	float rel_vel_unitary[2] = { rel_vel[0] / speed, rel_vel[1] / speed }; // Unitary vector of relative velocity
	float fdrag_modulus = ball.b * speed; // Drag force (modulus)

	if (hidroDragXEnabled)
	{
		fx = -rel_vel_unitary[0] * fdrag_modulus; // Drag is antiparallel to relative velocity
	}
	
	if (hidroDragYEnabled)
	{
		fy = -rel_vel_unitary[1] * fdrag_modulus; // Drag is antiparallel to relative velocity
	}
	
}

// Compute Hydrodynamic Buoyancy force
void ModulePhysics::compute_hydrodynamic_buoyancy(float& fx, float& fy, const PhysBall& ball, const Water& water)
{
	if (!buoyancyEnabled)
	{
		return;
	}

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
void ModulePhysics::integrator_velocity_verlet(PhysBall& ball, float dt)
{
	ball.x += ball.vx * dt + 0.5f * ball.ax * dt * dt;
	ball.y += ball.vy * dt + 0.5f * ball.ay * dt * dt;
	ball.vx += ball.ax * dt;
	ball.vy += ball.ay * dt;
}

void ModulePhysics::integrator_forward_euler(PhysBall& ball, double dt)
{
	ball.vx = ball.vx + ball.ax * dt;
	ball.vy = ball.vy + ball.ay * dt;

	ball.x = ball.x + ball.vx * dt;
	ball.y = ball.y + ball.vy * dt;
}

void ModulePhysics::integrator_backwards_euler(PhysBall& ball, double dt)
{
	ball.x = ball.x + ball.vx * dt;
	ball.y = ball.y + ball.vy * dt;

	ball.vx = ball.vx + ball.ax * dt;
	ball.vy = ball.vy + ball.ay * dt;
}


// Detect collision with ground
bool ModulePhysics::is_colliding_with_ground(const PhysBall& ball, const Ground& ground)
{
	float rect_x = (ground.x + ground.w / 2.0f); // Center of rectangle
	float rect_y = (ground.y + ground.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, ground.w, ground.h);
}

// Detect collision with wall
bool ModulePhysics::is_colliding_with_wall(const PhysBall& ball, const Wall& wall)
{
	float rect_x = (wall.x + wall.w / 2.0f); // Center of rectangle
	float rect_y = (wall.y + wall.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, wall.w, wall.h);
}

// Detect collision with sensor
bool ModulePhysics::is_colliding_with_sensor(const PhysBall& ball, const SensorWall& sensor)
{
	float rect_x = (sensor.x + sensor.w / 2.0f); // Center of rectangle
	float rect_y = (sensor.y + sensor.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, sensor.w, sensor.h);
}

// Detect collision with water
bool ModulePhysics::is_colliding_with_water(const PhysBall& ball, const Water& water)
{
	float rect_x = (water.x + water.w / 2.0f); // Center of rectangle
	float rect_y = (water.y + water.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, water.w, water.h);
}

// Detect collision between circle and rectange
bool ModulePhysics::check_collision_circle_rectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh)
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
	//printf("CANTONADESSSSSSSSSSSSS\n");
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

void PhysBall::AddForce(double forX, double forY)
{
	mfx = forX;
	mfy = forY;
}


//// Wall - ball collision solver OLD code:
//if (is_colliding_with_wall(ball, wall))
//{
//	// Ball is on right
//	//if(wall is at the left of the ball)&&(ball is between the edges of the wall collider)
//	if (ball.x - ball.radius < wall.x + wall.w && ball.x + ball.radius > wall.x + wall.w &&
//		ball.y > wall.y && ball.y + ball.radius < wall.y + wall.h)
//	{
//		if (ball.vx > -0.1f && ball.vx < 0.1f)
//		{
//			ball.x = wall.x + wall.w + ball.radius;
//			ball.vx = 0;
//		}
//		else
//		{
//			ball.x = (wall.x + wall.w) + ball.radius;
//			// Elastic bounce with wall
//			ball.vx = -ball.vx * wall.bouncyness;
//			ball.vy = ball.vy * wall.bouncyness;
//		}
//
//	}
//	//Ball is on left
//	//if (wall is at the right of the ball) && (ball is between the edges of the wall collider)
//	if (ball.x + ball.radius > wall.x && ball.x < wall.x &&
//		ball.y > wall.y && ball.y + ball.radius < wall.y + wall.h)
//	{
//		if (ball.vx > -0.1f && ball.vx < 0.1f)
//		{
//			ball.x = wall.x - ball.radius;
//			ball.vx = 0;
//		}
//		else
//		{
//			ball.x = wall.x - ball.radius;
//			// Elastic bounce with wall
//			ball.vx = -ball.vx * wall.bouncyness;
//			ball.vy = ball.vy * wall.bouncyness;
//		}
//	}
//	//Ball over wall
//	// if (ball is over the wall) &&  (ball is between the edges of the wall collider)
//	if (ball.y + ball.radius > wall.y && ball.y < wall.y && ball.x > wall.x - ball.radius && ball.x + ball.radius < wall.x + wall.w)
//	{
//		if (ball.vy > -0.1f && ball.vy < 0.1f)
//		{
//			ball.y = wall.y - ball.radius;
//			ball.vy = 0;
//		}
//		else
//		{
//			ball.y = wall.y - ball.radius;
//			// Elastic bounce with wall
//			ball.vx = ball.vx * wall.bouncyness;
//			ball.vy = -ball.vy * wall.bouncyness;
//		}					
//	}
//	// Ball under wall
//	// if (ball is under the wall) &&  (ball is between the edges of the wall collider)
//	if (ball.y - ball.radius < wall.y + wall.h && ball.y + ball.radius > wall.y + wall.h &&
//		ball.x > wall.x - ball.radius && ball.x + ball.radius < wall.x + wall.w)
//	{
//		if (ball.vy > -0.1f && ball.vy < 0.1f)
//		{
//			ball.y = wall.y + wall.h + ball.radius;
//			ball.vy = 0;
//		}
//		else
//		{
//			ball.y = wall.y + wall.h + ball.radius;
//			// Elastic bounce with wall
//			ball.vx = ball.vx * wall.bouncyness;
//			ball.vy = -ball.vy * wall.bouncyness;
//		}
//
//	}
//
//	// FUYM non-elasticity
//	ball.vx *= ball.coef_friction;
//	ball.vy *= ball.coef_restitution;
//}