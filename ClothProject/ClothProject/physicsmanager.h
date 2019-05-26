#pragma once
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody\btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

class PhysicsManager
{
public:
	PhysicsManager();
	~PhysicsManager();
	
	void Initialise();
	void Update(double _dTime);

	btAlignedObjectArray<btCollisionShape*>* GetCollisionShapes();
	btSoftRigidDynamicsWorld* GetWorld();
	btSoftBodyWorldInfo GetSoftInfo() const;

private:
	btDefaultCollisionConfiguration * collisionConfig;
	btCollisionDispatcher* collisionDispatcher;
	btBroadphaseInterface* broadphaseInterface;
	btSequentialImpulseConstraintSolver* solver;
	btSoftRigidDynamicsWorld* world;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	btSoftBodyWorldInfo softInfo;
};