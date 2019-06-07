#pragma once
#include "level.h"
#include "model.h"
#include "terrain.h"
#include "meshsphere.h"
#include "physicsmanager.h"

class MeshSphere;

class Sphere : public GameObject
{
public:
	Sphere(Level* _level, PhysicsManager* _physicsManager, Terrain * _terrain);
	virtual ~Sphere();

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
	MeshSphere* meshSphere;

	bool isHeld;
};
