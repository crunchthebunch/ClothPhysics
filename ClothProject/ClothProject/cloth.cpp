#include "cloth.h"

Cloth::Cloth(Level * _level, PhysicsManager * _physicsManager)
{
	level = _level;
	physics = _physicsManager;
	width = 32.0f;
	height = 32.0f;
	numRows = 8;
	numCols = 8;
	cellSpacing = width/numCols;
}

Cloth::~Cloth()
{
	//Delete cloth parts
	while (vecParts.size() > 0)
	{
		GameObject* temp = vecParts[vecParts.size() - 1];
		vecParts.pop_back();
		delete temp;
	}
}

void Cloth::Initialise()
{
	//Create grid of cloth parts constrained by spring constraints
	float halfW = width * 0.5f;
	float halfH = height * 0.5f;

	//Creating
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numCols; j++)
		{
			ClothPart* part;
			part = new ClothPart(level, physics);
			part->SetX((cellSpacing*j) - halfW);
			part->SetY((cellSpacing*i) + halfH);
			part->Initialise();
			vecParts.push_back(part);
		}
	}

	//Constraining
	ClothPart *partA, *partB;
	partA = FindPart(0, 0);

	for (int yy = 0; yy < numRows; yy++)
	{
		for (int xx = 1; xx < numCols; xx++)
		{
			partB = FindPart(xx,yy);

			if ((partA != nullptr && partB != nullptr) && (partA != partB))
			{
				CreateSpring(partA->GetBody(), partB->GetBody());
			}

			if (xx == numCols)
			{
				partA = nullptr;
			}
			else
			{
				partA = partB;
			}
		}
	}

	////Constraining
	//ClothPart *partC, *partD;
	//partC = FindPart(0, 0);

	//for (int xx = 0; xx < numCols; xx++)
	//{
	//	for (int yy = 0; yy < numRows; yy++)
	//	{
	//		partD = FindPart(xx, yy);

	//		if ((partC != nullptr && partD != nullptr) && (partC != partD))
	//		{
	//			CreateSpring(partC->GetBody(), partD->GetBody());
	//		}

	//		if (xx == numCols)
	//		{
	//			partC = nullptr;
	//		}
	//		else
	//		{
	//			partC = partD;
	//		}
	//	}
	//}
}

void Cloth::Update(double dTime)
{
	//Update cloth parts
	for (unsigned int i = 0; i < vecParts.size(); i++)
	{
		if (vecParts[i]->GetIsActive())
		{
			vecParts[i]->Update(deltaTime);
		}
	}
}

void Cloth::Draw()
{
	//Draw cloth parts
	for (unsigned int i = 0; i < vecParts.size(); i++)
	{
		if (vecParts[i]->GetIsActive())
		{
			vecParts[i]->Draw();
		}
	}
}

void Cloth::CreateSpring(btRigidBody * bodyA, btRigidBody * bodyB)
{
	//Spring constraint
	btGeneric6DofSpringConstraint *spring = new btGeneric6DofSpringConstraint(
		*bodyA, *bodyB,
		btTransform(btQuaternion::getIdentity(), { 4.0f, 4.0f, 4.0f }),
		btTransform(btQuaternion::getIdentity(), { 0.0f,  0.0f, 0.0f }),
		true
	);

	spring->setLinearLowerLimit(btVector3(1.0f, 1.0f, 1.0f));
	spring->setLinearUpperLimit(btVector3(4.0f, 4.0f, 4.0f));
	//spring->setAngularLowerLimit(btVector3(-15.0f, -15.0f, -15.0f));
	//spring->setAngularUpperLimit(btVector3(15.0f, 15.0f, 15.0f));

	//Enable X
	spring->enableSpring(0, true);
	spring->setStiffness(0, 5.0f);
	spring->setDamping(0, 0.5f);

	//Enable Y
	spring->enableSpring(1, true);
	spring->setStiffness(1, 5.0f);
	spring->setDamping(1, 0.5f);

	//Enable Z
	spring->enableSpring(2, true);
	spring->setStiffness(2, 5.0f);
	spring->setDamping(2, 0.5f);

	////Enable X
	//spring->enableSpring(3, true);
	//spring->setStiffness(3, 35.0f);
	//spring->setDamping(3, 0.5f);

	////Enable Y
	//spring->enableSpring(4, true);
	//spring->setStiffness(4, 35.0f);
	//spring->setDamping(4, 0.5f);

	////Enable Z
	//spring->enableSpring(5, true);
	//spring->setStiffness(5, 35.0f);
	//spring->setDamping(5, 0.5f);
	
	spring->setEquilibriumPoint();

	vecSprings.push_back(spring);
	physics->GetWorld()->addConstraint(spring);
}

ClothPart * Cloth::FindPart(int _x, int _y)
{
	ClothPart* part = vecParts[_x + (_y * numCols)];
	return part;
}
