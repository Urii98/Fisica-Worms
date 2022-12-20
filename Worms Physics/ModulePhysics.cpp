#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"

#include "math.h"

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
	char lookupTableChars[] = { " !'#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[/]^_ abcdefghijklmnopqrstuvwxyz{|}~ çüéâäàaçêëèïîìäaéÆæôöòûù" };
	textFont = App->fonts->Load("Assets/pixel_font.png", lookupTableChars, 8);
	winc = true;

	ground = new Ground();
	ground->x = PIXELS_TO_METERS(0);
	ground->y = PIXELS_TO_METERS(500);
	ground->w = PIXELS_TO_METERS(500);
	ground->h = PIXELS_TO_METERS(250);

	ground->ground_col = App->col->AddCollider({ ground->x, ground->y, ground->w ,ground->h }, Collider::Type::GROUND, this);


	platform = new Ground();

	platform->x = PIXELS_TO_METERS(1200);
	platform->y = PIXELS_TO_METERS(500);
	platform->w = PIXELS_TO_METERS(100);
	platform->h = PIXELS_TO_METERS(250);

	platform->ground_col = App->col->AddCollider({ platform->x, platform->y, platform->w ,platform->h }, Collider::Type::GROUND, this);


	agua = new Ground();

	agua->x = PIXELS_TO_METERS(500);
	agua->y = PIXELS_TO_METERS(500);
	agua->w = PIXELS_TO_METERS(700);
	agua->h = PIXELS_TO_METERS(250);


	isla = new Ground();

	isla->x = PIXELS_TO_METERS(600);
	isla->y = PIXELS_TO_METERS(100);
	isla->w = PIXELS_TO_METERS(200);
	isla->h = PIXELS_TO_METERS(100);

	isla->ground_col = app->col->AddCollider({ isla->x, isla->y, isla->w ,isla->h }, Collider::Type::GROUND, this);


	win = new Ground();

	win->x = PIXELS_TO_METERS(1200);
	win->y = PIXELS_TO_METERS(400);
	win->w = PIXELS_TO_METERS(50);
	win->h = PIXELS_TO_METERS(50);


	integer = 1;
	movement = 1;

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	return true;
}


PhysBody* ModulePhysics::CreateCircle(int x, int y, int r, int mass) {

	PhysBody* circle = new PhysBody();

	/*circle->app = App;*/

	circle->data.posX = x;
	circle->data.posY = y;
	circle->data.radi = r;
	circle->data.mass = mass;

	circle->physicObject = true;

	circle->isCircle = true;

	return circle;

}