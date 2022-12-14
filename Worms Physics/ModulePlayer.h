#pragma once
#include "Module.h"
#include "Animation.h"
#include "ModulePhysics.h"
#include "Globals.h"
#include "p2Point.h"

class PhysBall;

enum MoveType
{
	NONE,
	POSITION,
	FORCES,
	VELOCITY
};

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	PhysBall body;

	bool onGround;

	int ballIdentification;

	float angle_shot;
	float strength;
	float objective_x;
	float objective_y;

	MoveType moveType;

	float dt;
	bool isDead;

	bool introParameters;

	int score;
};