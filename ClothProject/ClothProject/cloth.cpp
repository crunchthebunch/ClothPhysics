#include "cloth.h"
#include "cubemap.h"

Cloth::Cloth(Level * _level, PhysicsManager * _physicsManager)
{
	level = _level;
	physics = _physicsManager;
	camera = level->GetCamera();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetAssetLoader();
	skyBoxTexture = level->GetSkyBox()->GetTexture();
	width = 32.0f;
	height = 32.0f;
	numRows = 15;
	numCols = 15;
	cellSpacing = width/numCols;
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;
}

Cloth::~Cloth()
{
	//Delete springs
	while (vecSprings.size() > 0)
	{
		btGeneric6DofConstraint* temp = vecSprings[vecSprings.size() - 1];
		vecSprings.pop_back();
		delete temp;
	}

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
			part = new ClothPart(level, physics, this, i, j);
			part->SetX((cellSpacing*j) - halfW);
			part->SetY((cellSpacing*i) + halfH);

			if (i == numRows - 1 && Utils::IsEven(j) == true)
			{
				part->SetIsStatic(true);
			}
			part->Initialise();

			vecParts.push_back(part);
			vecPickable->push_back(part);
		}
	}

	for (auto i = vecParts.begin(); i != vecParts.end(); i++)
	{
		ClothPart* temp = *i;
		temp->SetParts();
	}

	ConstrainParts();
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

	//Disable torn parts
	for (unsigned int i = 0; i < vecSprings.size(); i++)
	{
		if (vecSprings[i]->getAppliedImpulse() >= 2.5f)
		{
			vecSprings[i]->setEnabled(false);
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


std::vector<ClothPart*> Cloth::getVecParts()
{
	return vecParts;
}

int Cloth::getNumRows()
{
	return numRows;
}

int Cloth::getNumCols()
{
	return numCols;
}

void Cloth::setNumRows(int value)
{
	numRows = value;
}

void Cloth::setNumCols(int value)
{
	numCols = value;
}

void Cloth::CreateSpring(btRigidBody * bodyA, btRigidBody * bodyB)
{
	float halfCell = cellSpacing * 0.5f;
	//Spring constraint
	btGeneric6DofSpringConstraint *spring = new btGeneric6DofSpringConstraint(
		*bodyA, *bodyB,
		btTransform(btQuaternion::getIdentity(), { halfCell, halfCell, halfCell }),
		btTransform(btQuaternion::getIdentity(), { halfCell, halfCell, halfCell }),
		true
	);

	spring->setLinearLowerLimit(btVector3(2.0f*-cellSpacing, 2.0f*-cellSpacing, 2.0f*-cellSpacing));
	spring->setLinearUpperLimit(btVector3(1.0f*cellSpacing, 1.0f*cellSpacing, 1.0f*cellSpacing));
	spring->setAngularLowerLimit(btVector3(-0.0f, -0.0f, -0.0f));
	spring->setAngularUpperLimit(btVector3(0.0f, 0.0f, 0.0f));

	//Enable X
	spring->enableSpring(0, true);
	spring->setStiffness(0, 1.0f);
	spring->setDamping(0, 5.0f);

	//Enable Y
	spring->enableSpring(1, true);
	spring->setStiffness(1, 1.0f);
	spring->setDamping(1, 5.0f);

	//Enable Z
	spring->enableSpring(2, true);
	spring->setStiffness(2, 1.0f);
	spring->setDamping(2, 5.0f);

	//Enable X
	/*spring->enableSpring(3, true);
	spring->setStiffness(3, 5.0f);
	spring->setDamping(3, 1.0f);*/

	//Enable Y
	//spring->enableSpring(4, true);
	//spring->setStiffness(4, 5.0f);
	//spring->setDamping(4, 1.0f);

	////Enable Z
	//spring->enableSpring(5, true);
	//spring->setStiffness(5, 5.0f);
	//spring->setDamping(5, 100.0f);
	
	spring->setEquilibriumPoint();

	vecSprings.push_back(spring);
	physics->GetWorld()->addConstraint(spring);
}

void Cloth::ConstrainParts()
{
	//Constraining Horizontal Springs
	ClothPart *partA, *partB;
	partA = FindPart(0, 0);

	for (int yy = 0; yy < numRows; yy++)
	{
		for (int xx = 0; xx < numCols; xx++)
		{
			partB = FindPart(xx, yy);

			if ((partA != nullptr && partB != nullptr) && (partA != partB))
			{
				CreateSpring(partA->GetBody(), partB->GetBody());
			}

			if (xx == numCols - 1)
			{
				partA = nullptr;
			}
			else
			{
				partA = partB;
			}
		}
	}

	//Constraining Vertical Springs
	partA = FindPart(0, 0);

	for (int yy = 0; yy < numRows; yy++)
	{
		for (int xx = 0; xx < numCols; xx++)
		{
			partB = FindPart(yy, xx);

			if ((partA != nullptr && partB != nullptr) && (partA != partB))
			{
				CreateSpring(partA->GetBody(), partB->GetBody());
			}

			if (xx == numCols - 1)
			{
				partA = nullptr;
			}
			else
			{
				partA = partB;
			}
		}
	}

	//Constraining Diagonal "\" Springs
	for (int yy = 0; yy < numRows; yy++)
	{
		for (int xx = 0; xx < numCols; xx++)
		{
			if (yy + 1 < numRows - 1 && xx + 1 < numCols - 1)
			{
				partA = FindPart(xx, yy);
				partB = FindPart(xx + 1, yy + 1);
				CreateSpring(partA->GetBody(), partB->GetBody());
			}
			else
			{
				break;
			}
		}
	}

	//Constraining Diagonal "/" Springs
	for (int yy = 0; yy < numRows; yy++)
	{
		for (int xx = 0; xx < numCols; xx++)
		{
			if (yy - 1 >= 0 && xx - 1 >= 0)
			{
				partA = FindPart(xx, yy);
				partB = FindPart(xx - 1, yy - 1);
				CreateSpring(partA->GetBody(), partB->GetBody());
			}
			else
			{
				break;
			}
		}
	}
}

ClothPart * Cloth::FindPart(int _x, int _y)
{
	ClothPart* part = vecParts[_x + (_y * numCols)];
	return part;
}


void Cloth::SetVecPickable(std::vector<GameObject*>* _vecPickable)
{
	vecPickable = _vecPickable;
}