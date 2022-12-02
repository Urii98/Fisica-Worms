#pragma once
#include "Globals.h"

struct Data
{
	float radi = 0.0f;
	float posX = 0.0f, posY = 0.0f;
	float width = 0.0f, height = 0.0f;
	float mass = 0.0f;
	float velX = 0.0f, velY = 0.0f;
	float accX = 0.0f, accY = 0.0f;
	float forceX = 0.0f, forceY = 0.0f;
	float gravForceX = 0.0f, gravForceY = 0.0f;
	float dragFoceX = 0.0f, dragForceY = 0.0f;
	float relativeVelX = 0.0f, relativeVelY = 0.0f;

	float densitat = 1.0f;

	float dragSurface = 0.3f;

	float dragCoefficient = 0.2f;

	//fb = f_density * volume * 9.81
	//density of water = 1g/cm^3 = 1000kg/m^3
	//volume = volumen sumergido del objeto
	float fb = 0, f_density = 1, volume = 0;

	float cr = 0.6f;//Coeficiente de restitucion
	float cf = 0.8f;//Coeficiente de friccon
};

class PhysBody
{
public:

	Data data;

	bool physicObject = false;
	bool physEnable = true;

	bool circle = false;
	bool rectangle = false;
	bool player = false;
	bool water = false;

	bool force = false;

	bool colliding = false;

private:

	PhysBody();
	~PhysBody();

	float angleToRadiants(int angle);

	//Modifica la posición del objeto
	void SetPosition(int x, int y);

	//Modifica la masa del objeto
	void SetMass(int mass);

	//Modifica la velocidad del objeto
	void SetVelocity(int x, int y);

	//Modifica la acceleracion del objeto
	void SetAcceleration(int x, int y);

	//Modifica la fuerza dada al objeto
	void SetForce(int x, int y);


	void StopPhysics();

	//Te devuelve la masa del objeto
	int GetMass();


};