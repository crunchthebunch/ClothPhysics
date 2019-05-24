#pragma once
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftBody.h"

class PhysicsManager
{
public:
	PhysicsManager();
	~PhysicsManager();
	
	void Initialise();
	void Update(double _dTime);

private:
	btDynamicsWorld* world; //Physics world
};