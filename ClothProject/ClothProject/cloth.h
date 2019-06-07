#pragma once
#include "level.h"
#include "physicsmanager.h"
#include "clothpart.h"

//Cloth itself is not an object, but instead is a "manager" for a vector of cloth parts (spheres) that
//are joined together by constraints so that the cloth can collide and react like cloth.
//This class will also be used to generate and draw the mesh of the cloth.

class Cloth : public GameObject
{
public:
	Cloth(Level* _level, PhysicsManager* _physicsManager);
	virtual ~Cloth();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	int getNumRows();
	int getNumCols();

	void SetVecPickable(std::vector<GameObject*>* _vecPickable);
	ClothPart* FindPart(int _x, int _y);

private:
	void CreateSpring(btRigidBody* bodyA, btRigidBody* bodyB);
	void ConstrainParts();
	

private:
	PhysicsManager * physics;
	Level* level;
	std::vector<ClothPart*> vecParts;
	std::vector<btGeneric6DofConstraint*> vecSprings;
	std::vector<GameObject*>* vecPickable;

	float width;
	float height;
	int numRows;
	int numCols;



	Mesh* clothMesh;

	float cellSpacing;
};