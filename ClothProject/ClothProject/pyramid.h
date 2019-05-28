#pragma once
#include "level.h"
#include "model.h"
#include "terrain.h"
#include "physicsmanager.h"

class Pyramid : public GameObject
{
public:
	Pyramid(Level* _level, PhysicsManager* _physicsManager, Terrain * _terrain);
	virtual ~Pyramid();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	virtual void MousePressing();

private:
	PhysicsManager* physics;
	btCollisionShape* colShape;
	btRigidBody* body;
	Level* level;
	Model* model;
	Model* stencil;
	Terrain* terrain;

	bool isHeld;
};
