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
#include "ModuleFonts.h"

#include "Timer.h"
#include "PerfTimer.h"

class ModuleFonts; 

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
	ModuleFonts* fonts;

private:

	p2List<Module*> list_modules;
	uint frames;
	float dt;

	// L13: TODO 4: Calculate some timing measures
	// required variables are provided:
	Timer timer;
	PerfTimer ptimer;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	uint64 frameCount = 0;
	uint32 framesPerSecond = 0;
	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	float secondsSinceStartup = 0.0f;

	uint32 maxFrameDuration = 0;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void FinishUpdate();
	void PrepareUpdate();
	

private:

	void AddModule(Module* mod);
};