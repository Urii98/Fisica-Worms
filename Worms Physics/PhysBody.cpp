#include "Globals.h"
#include "math.h"
#include "PhysBody.h"
#include "Application.h"

PhysBody::PhysBody() {

}

PhysBody::~PhysBody() {

}

float PhysBody::angleToRadiants(int angle) {

	float radiants;

	radiants = angle * M_PI / 180;
	return radiants;
}

void PhysBody::SetPosition(int x, int y) {
	data.posX = x;
	data.posY = y;
}
void PhysBody::SetMass(int mass) {
	data.mass = mass;
}

int PhysBody::GetMass() {
	return data.mass;
}

void PhysBody::SetVelocity(int x, int y) {

	data.velX = x;
	data.velY = y;
}


void PhysBody::SetAcceleration(int x, int y) {
	data.accX = x;
	data.accY = y;

}

void PhysBody::SetForce(int x, int y) {

	data.forceX = x;
	data.forceY = y;
};

//void PhysBody::StopPhysics() {
//	if (physEnable) {
//		physEnable = false;
//	}
//	else if (!physEnable) {
//		physEnable = true;
//	}
//}
