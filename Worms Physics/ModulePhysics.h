#pragma once
#include "Module.h"
#include "Globals.h"
#include "PhysBody.h"

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();
	PhysBody* CreateCircle(int x, int y, int r, int mass);
private:

	bool debug;

	PhysBody* body;
};