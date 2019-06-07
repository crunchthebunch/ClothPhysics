#pragma once
#include "level.h"
#include "physicsmanager.h"
#include "meshsphere.h"
//#include "cloth.h"

class Cloth;
//Cloth Part is a rigidbody sphere collider that will be constrained to other cloth parts to create a collidable cloth

class ClothPart : public GameObject
{
public:
	ClothPart(Level* _level, PhysicsManager* _physicsManager, Cloth* cloth, int row, int col);
	virtual ~ClothPart();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	virtual void MousePressing();

	void SetIsStatic(bool _static);
	btRigidBody* GetBody();

	int row;
	int col;

	void SetParts();

private:
	PhysicsManager * physics;
	btRigidBody* body;
	Level* level;
	MeshSphere* sphere;
	bool isDebugDraw;
	bool isStatic;
	bool isHeld;
	bool inCorner;

	ClothPart* part1;
	ClothPart* part2;
	ClothPart* part3;

	Cloth* cloth;

};