#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Module.h"
#include "Defs.h"
#include "Log.h"
#include "DynArray.h"
#include "Point.h"

#define METERS_PER_PIXEL 0.02f
#define PIXELS_PER_METER 50.0f

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXELS_TO_METERS(p) ((float) METERS_PER_PIXEL * p)
#define GRAVITY 10.0f

class Ball
{
public:
	// Position
	// You could also use an array/vector
	double x;
	double y;
	double rad;

	// Velocity
	double vx;
	double vy;

	// Acceleration
	double ax;
	double ay;

	//Angle
	double angle = sqrt(vx * vx + vy * vy);

	// Force (total) applied to the ball
	double fx;
	double fy;

	// Mass
	double mass;

	// Aerodynamics stuff
	double surface; // Effective wet surface
	double cl; // Lift coefficient
	double cd = 0.47f; // Drag coefficient
	
	//Hidrodinamic stuff
	double density;
	double volume; //volumen submergido


	// Has physics enabled?
	bool physics_enabled = true;
	

	void SetVelocity(double vx, double vy);
	void AddForce(double fx, double fy);
	void SetPosition(double x, double y);

	dPoint GetVelocity();

	double mfx;
	double mfy;

	Collider* ball_col = nullptr;

};

class Ground
{
public:
	double x;
	double y;
	double w;
	double h;
	double density = 400;
	Collider* ground_col = nullptr;
};

class Physics : public Module
{
public:
	Physics();

	virtual ~Physics();

	bool Awake();

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void Integrator_velocity_verlet(Ball* ball, double dt);
	void Integrator_forward_euler(Ball* ball, double dt);
	void Integrator_backwards_euler(Ball* ball, double dt);

	int CreateBall(double mass, double rad, double x, double y, double vx, double vy);

	void OnCollision(Collider* c1, Collider* c2) override;

	DynArray<Ball> balls;
	Ground* ground;
	Ground* platform;
	Ground* agua;
	Ground* isla;
	Ground* win;

	int movement;

	bool onCol = false;

	int textFont = -1;

	bool winc = false;

	// Has physics enabled?
	bool gravityEnabled = true;
	bool buoyancyEnabled = true;
	bool hidroDragEnabled = true;
	bool aeroDragEnabled = true;

private:

	bool debug;

	int integer;

	
};

#endif