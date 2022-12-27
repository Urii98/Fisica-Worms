#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleFonts.h"
#include "ModuleSceneIntro.h"





ModuleFonts::ModuleFonts(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleFonts::~ModuleFonts()
{

}

// Load assets
bool ModuleFonts::Start()
{
	LOG("Loading Fonts");

	char lookupTableChars[] = { " !'#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[/]^_ abcdefghijklmnopqrstuvwxyz{|}~ ������a��������a��������" };
	textFont = App->fonts->Load("Assets/pixel_font.png", lookupTableChars, 8);



	return true;
}

// Load new texture from file path
int ModuleFonts::Load(const char* texture_path, const char* characters, uint rows)
{
	int id = -1;

	if(texture_path == nullptr || characters == nullptr || rows == 0)
	{
		LOG("Could not load font");
		return id;
	}

	SDL_Texture* tex = App->textures->Load(texture_path);

	if(tex == nullptr || strlen(characters) >= MAX_FONT_CHARS)
	{
		LOG("Could not load font at %s with characters '%s'", texture_path, characters);
		return id;
	}

	id = 0;
	for(; id < MAX_FONTS; ++id)
		if(fonts[id].texture == nullptr)
			break;

	if(id == MAX_FONTS)
	{
		LOG("Cannot load font %s. Array is full (max %d).", texture_path, MAX_FONTS);
		return id;
	}

	Font& font = fonts[id];

	font.texture = tex;
	font.rows = rows;

	// TODO 1: Finish storing font data

	// totalLength ---	length of the lookup table
	// table ---------  All characters displayed in the same order as the texture
	// columns -------  Amount of chars per row of the texture
	// char_w --------	Width of each character
	// char_h --------	Height of each character

	strcpy_s(fonts[id].table, MAX_FONT_CHARS, characters);
	font.totalLength = strlen(characters);
	font.columns = fonts[id].totalLength / rows;

	uint tex_w, tex_h;
	App->textures->GetTextureSize(tex, tex_w, tex_h);
	font.char_w = tex_w / font.columns;
	font.char_h = tex_h / font.rows;

	LOG("Successfully loaded BMP font from %s", texture_path);

	return id;
}

void ModuleFonts::UnLoad(int font_id)
{
	if(font_id >= 0 && font_id < MAX_FONTS && fonts[font_id].texture != nullptr)
	{
		App->textures->Unload(fonts[font_id].texture);
		fonts[font_id].texture = nullptr;
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
}

void ModuleFonts::BlitText(int x, int y, int font_id, const char* text) const
{
	if(text == nullptr || font_id < 0 || font_id >= MAX_FONTS || fonts[font_id].texture == nullptr)
	{
		LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}

	const Font* font = &fonts[font_id];
	SDL_Rect spriteRect;
	uint len = strlen(text);

	spriteRect.w = font->char_w;
	spriteRect.h = font->char_h;

	for(uint i = 0; i < len; ++i)
	{
		// TODO 2: Find the character in the table and its position in the texture, then Blit
		uint charIndex = 0;

		// Find the location of the current character in the lookup table
		for (uint j = 0; j < font->totalLength; ++j)
		{
			if (font->table[j] == text[i])
			{
				charIndex = j;
				break;
			}
		}

		// Retrieve the position of the current character in the sprite
		spriteRect.x = spriteRect.w * (charIndex % font->columns);
		spriteRect.y = spriteRect.h * (charIndex / font->columns);

		App->renderer->Blit(font->texture, x, y, &spriteRect, 0.0f, false);

		// Advance the position where we blit the next character
		x += spriteRect.w;
	}
}

std::string ModuleFonts::PhysicsParamsToString(float param, int decimales, const char* text)
{
	std::string paramToString = std::to_string(param);
	std::string rounded = paramToString.substr(0, paramToString.find(".") + decimales +1);
	std::string typeOfParam = text;

	std::string result;
	result.append(typeOfParam);
	result.append(rounded);

	return result;
}

update_status ModuleFonts::Update()
{

	//URI - TO DO'S: Meter los parametros dentro del ground

	//URI - TO DO'S - 2: Hacer que se puedan modificar a tiempo real todos estos par�metros

	
	//Gravity
	std::string gravString = PhysicsParamsToString(App->physics->gravity, 2, "Gravity    : ");
	const char* gravChar = gravString.c_str();
	BlitText(0, 0, textFont, gravChar); 

	//AeroDrag
	std::string aDragXString = PhysicsParamsToString(App->physics->aeroDragX, 2, "A. Drag.x  : ");
	const char* aDragXChar = aDragXString.c_str();
	BlitText(0, 17, textFont, aDragXChar);

	std::string aDragYString = PhysicsParamsToString(App->physics->aeroDragY, 2, "A. Drag.y  : ");
	const char* aDragYChar = aDragYString.c_str();
	BlitText(0, 34, textFont, aDragYChar);

	//HidroDrag
	std::string hDragXString = PhysicsParamsToString(App->physics->hidroDragX, 2, "H. Drag.x  : ");
	const char* hDragXChar = hDragXString.c_str();
	BlitText(0, 51, textFont, hDragXChar);

	std::string hDragYString = PhysicsParamsToString(App->physics->hidroDragY, 2, "H. Drag.y  : ");
	const char* hDragYChar = hDragYString.c_str();
	BlitText(0, 68, textFont, hDragYChar);

	//Hidro Buoyancy
	std::string hBuoyancyString = PhysicsParamsToString(App->physics->buoyancy, 2, "H. Buoya.  : ");
	const char* hBuoyancyChar = hBuoyancyString.c_str();
	BlitText(0, 85, textFont, hBuoyancyChar); 


	//Atmosphere Params
	std::string windXString = PhysicsParamsToString(App->scene_intro->atmosphere.windx, 2, "Wind.x     : ");
	const char* windXChar = windXString.c_str();
	BlitText(0, 112, textFont, windXChar);

	std::string windYString = PhysicsParamsToString(App->scene_intro->atmosphere.windy, 2, "Wind.y     : ");
	const char* windYChar = windYString.c_str();
	BlitText(0, 129, textFont, windYChar);

	std::string atmosphDensityString = PhysicsParamsToString(App->scene_intro->atmosphere.density, 2, "Atmsph.D.  : "); 
	const char* atmosphDensityChar = atmosphDensityString.c_str();
	BlitText(0, 146, textFont, atmosphDensityChar);

	//Water Params
	std::string waterVelXString = PhysicsParamsToString(App->scene_intro->water1.vx, 2, "Water Vel.x: ");
	const char* waterVelXChar = waterVelXString.c_str();
	BlitText(0, 163, textFont, waterVelXChar);

	std::string waterVelYString = PhysicsParamsToString(App->scene_intro->water1.vy, 2, "Water Vel.y: ");
	const char* waterVelYChar = waterVelYString.c_str();
	BlitText(0, 180, textFont, waterVelYChar);

	std::string waterDensityString = PhysicsParamsToString(App->scene_intro->water1.density, 2, "Water.D.   : ");
	const char* waterDensityChar = waterDensityString.c_str();
	BlitText(0, 197, textFont, waterDensityChar);

	return UPDATE_CONTINUE;
}
