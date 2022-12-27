
#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "Box2D/Box2D/Box2D.h"

PhysBody::PhysBody(b2Body* body, const SDL_Rect& rect, body_type type) : body(body), rect(rect), type(type), listener(NULL)
{}

PhysBody::~PhysBody()
{
	body->GetWorld()->DestroyBody(body);
	body = NULL;
	listener = NULL;
}

double PhysBody::GetAngle() const
{
	return RADTODEG * body->GetAngle();
}

void PhysBody::GetPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x)- rect.w / 2;
	y = METERS_TO_PIXELS(pos.y)- rect.h / 2;
}

void PhysBody::SetLinearSpeed(int x, int y)
{
	body->SetLinearVelocity(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)));
}

void PhysBody::SetAngularSpeed(float speed)
{
	body->SetAngularVelocity(speed * DEGTORAD);
}

void PhysBody::Push(float x, float y)
{
	body->ApplyForceToCenter(b2Vec2(x, y), true);
}

void PhysBody::Turn(int degrees)
{
	body->ApplyAngularImpulse(DEGTORAD * degrees, true);
}

void PhysBody::SetPosition(int x, int y)
{
	body->SetTransform(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)), 0.0f);
}
