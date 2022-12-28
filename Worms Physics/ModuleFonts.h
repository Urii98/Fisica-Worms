#pragma once

#include "Module.h"
#include <iostream>
#include <string>
#include <list>
#include <vector>


#define MAX_FONTS 10
#define MAX_FONT_CHARS 256

struct SDL_Texture;

struct Font
{
	// Lookup table. All characters displayed in the same order as the texture
	char table[MAX_FONT_CHARS];

	// The font texture
	SDL_Texture* texture = nullptr;

	// Font setup data
	uint totalLength;
	uint rows, columns;
	uint char_w, char_h;
};

class ModuleFonts : public Module
{
public:

	// Constructor
	ModuleFonts(Application* app, bool start_enabled = true);

	// Destructor
	~ModuleFonts();
	
	bool Start();
	update_status Update();
	update_status PostUpdate();

	void initialValuePhysParams();

	// Loads a font file from a texture
	// Returns a font index from the fonts array
	// Param texturePath	- The path to the texture file
	// Param characters		- The lookup table. All characters displayed in the same order as the texture
	// Param rows			- The amount of character rows in the texture
	int Load(const char* texturePath, const char* characters, uint rows = 1);

	// Removes a font by its index
	// Unloads the texture and removes it from the fonts array
	void UnLoad(int fontIndex);

	// Create a surface from text
	void BlitText(int x, int y, int fontIndex, const char* text) const;

	std::string PhysicsParamsToString(float param, int decimales, const char* text);

private:
	// An array to keep track and store all loaded fonts
	Font fonts[MAX_FONTS];

public:
	bool debug;

	int textFont = -1;
	int textFont2 = -1;
	int textFont3 = -1;

	enum enumPhysParams {
		GRAVITY,
		ATMOWINDX,
		ATMOWINDY,
		ATMODENSITY,
		WATERVX,
		WATERVY,
		WATERDENSITY,
		WALLSBOUNCYNESS,
		WALLSBOUNCYNESS2,
		BALLMASS,
		BALLFRICTION,
		BALLRESTITUION,
	};

	std::vector<int> physParams;
	int iterador;
	float toSum;
	bool reset;


	float initialGravity;
	float initialAtmoWindx, initialAtmoWindy, initialAtmoDensity;
	float initialWaterVelx, initialWaterVely, initialWaterDensity;
	float initialWallBouncyness;
	float initialBallMass, initialBallFriction, initialBallRestitution;

};

