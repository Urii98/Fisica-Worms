#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Module.h"
#include "Dummy.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"
#include "Collider.h"
#include "Collisions.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "List.h"
#include "SString.h"
#include "Fonts.h"

class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleAudio* audio;
	ModulePlayer* player;
	ModuleSceneIntro* scene_intro;
	ModulePhysics* physics;
	Collisions* collisions;
	Fonts* fonts;

private:

	p2List<Module*> list_modules;

	int argc;
	char** args;
	SString title;
	SString organization;

	List<Module*> modules;

	PerfTimer* ptimer;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	uint64 frameCount = 0;
	uint32 framesPerSecond = 0;
	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	float dt = 0.0f;

	uint32 maxFrameRate;

	uint32 changeFrameRate;

	PerfTimer* frameDuration;;

public:

	Application();
	~Application();

	bool Init();
	void PrepareUpdate();
	void FinishUpdate();
	update_status Update();
	bool CleanUp();
	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void ChangeFps();
private:

	void AddModule(Module* mod);
};