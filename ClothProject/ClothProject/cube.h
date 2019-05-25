#pragma once
#include "level.h"
#include "model.h"
#include "terrain.h"
#include "physicsmanager.h"

class Cube : public GameObject
{
public:
	Cube(Level* _level, PhysicsManager* _physicsManager, Terrain * _terrain);
	virtual ~Cube();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

private:
	PhysicsManager* physics;
	btCollisionShape* colShape;
	btRigidBody* body;
	Level* level;
	Model* model;
	Model* stencil;
	Terrain* terrain;
};