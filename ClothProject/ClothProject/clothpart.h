#pragma once
#include "level.h"
#include "physicsmanager.h"
#include "meshsphere.h"

//Cloth Part is a rigidbody sphere collider that will be constrained to other cloth parts to create a collidable cloth

class ClothPart : public GameObject
{
public:
	ClothPart(Level* _level, PhysicsManager* _physicsManager);
	virtual ~ClothPart();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void SetIsStatic(bool _static);
	btRigidBody* GetBody();

private:
	PhysicsManager * physics;
	btRigidBody* body;
	Level* level;
	MeshSphere* sphere;
	bool isDebugDraw;
	bool isStatic;
};