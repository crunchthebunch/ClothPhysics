#pragma once
#include "level.h"
#include "model.h"
#include "terrain.h"
#include "physicsmanager.h"

class Capsule : public GameObject
{
public:
	Capsule(Level* _level, PhysicsManager* _physicsManager, Terrain * _terrain);
	virtual ~Capsule();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	virtual void MousePressing();

	void Reset();

private:
	PhysicsManager* physics;
	btCollisionShape* colShape;
	btRigidBody* body;
	Level* level;
	Model* model;
	Model* stencil;
	Terrain* terrain;
	btTransform startTransform;
	bool isHeld;
};
#pragma once
