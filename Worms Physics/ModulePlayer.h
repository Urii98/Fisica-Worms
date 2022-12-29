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

	MoveType moveType;

	int score;
};