#include "physicsmanager.h"

PhysicsManager::PhysicsManager()
{
	gravity = -10.0f;
}

PhysicsManager::~PhysicsManager()
{
	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		world->removeCollisionObject(obj);
		delete obj;
	}

	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	delete collisionConfig;
	delete collisionDispatcher;
	delete broadphaseInterface;
	delete solver;
	delete world;
}

void PhysicsManager::Initialise()
{
	collisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();
	collisionDispatcher = new btCollisionDispatcher(collisionConfig);
	broadphaseInterface = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	world = new btSoftRigidDynamicsWorld(collisionDispatcher, broadphaseInterface, solver, collisionConfig);
	world->setGravity(btVector3(0.0f, gravity, 0.0f));

	softInfo.m_broadphase = broadphaseInterface;
	softInfo.m_dispatcher = collisionDispatcher;
	softInfo.m_gravity = world->getGravity();
	softInfo.m_sparsesdf.Initialize();
}

void PhysicsManager::Update(double _dTime)
{
	world->stepSimulation((btScalar)_dTime, 10);
}

btAlignedObjectArray<btCollisionShape*>* PhysicsManager::GetCollisionShapes()
{
	return &collisionShapes;
}

btSoftRigidDynamicsWorld * PhysicsManager::GetWorld()
{
	return world;
}

btSoftBodyWorldInfo PhysicsManager::GetSoftInfo() const
{
	return softInfo;
}
