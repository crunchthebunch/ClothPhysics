#pragma once
#include "btBulletDynamicsCommon.h"

class PhysicsManager
{
public:
	PhysicsManager();
	~PhysicsManager();
	
	void Initialise();
	void Update(double _dTime);

	btAlignedObjectArray<btCollisionShape*>* GetCollisionShapes();
	btDynamicsWorld* GetWorld();

private:
	btDefaultCollisionConfiguration * collisionConfig;
	btCollisionDispatcher* collisionDispatcher;
	btBroadphaseInterface* broadphaseInterface;
	btSequentialImpulseConstraintSolver* solver;
	btDynamicsWorld* world;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
};