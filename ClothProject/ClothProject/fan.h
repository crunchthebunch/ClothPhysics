#pragma once
#include "level.h"
#include "model.h"
#include "terrain.h"
#include "cloth.h"
#include "physicsmanager.h"
#include "INIParser.h"

class Fan : public GameObject
{
public:
	Fan(Level* _level, PhysicsManager* _physicsManager, Terrain * _terrain, Cloth* cloth, INIParser* iniParser);
	virtual ~Fan();

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
	Terrain* terrain;
	btTransform startTransform;

	float windBoxX;
	float windBoxY;
	float windBoxZ;

	INIParser* iniParser;

	Cloth* cloth;

	bool isHeld;
};