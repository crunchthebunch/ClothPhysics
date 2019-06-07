#include "clothpart.h"
#include "cubemap.h"
#include "cloth.h"

ClothPart::ClothPart(Level * _level, PhysicsManager * _physicsManager, Cloth* cloth, int row, int col)
{
	this->level = _level;
	this->cloth = cloth;
	this->row = col;
	this->col = row;

	VBO = level->GetVBO();
	camera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetAssetLoader();
	skyBoxTexture = level->GetSkyBox()->GetTexture();
	vecTexts = level->GetVecTexts();
	clock = level->GetClock();
	physics = _physicsManager;
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;
	isDebugDraw = true;
	isStatic = false;
	inCorner = false;
}

ClothPart::~ClothPart()
{
	delete sphere;
}

void ClothPart::Initialise()
{
	sphere = new MeshSphere(this, skyBoxTexture);
	sphere->InitMesh("Assets/cube.png", "cube", 0.0f, 1, 1, 1, 0, 1024, 1024);



	//Physics Sphere
	btCollisionShape* colShape = new btSphereShape(btScalar(1.0f));
	physics->GetCollisionShapes()->push_back(colShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(0.0f);

	btVector3 localInertia(0, 0, 0);

	if (!isStatic)
	{
		mass = 0.1f;
		colShape->calculateLocalInertia(mass, localInertia);
	}

	startTransform.setOrigin(btVector3(x, y, z));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	body = new btRigidBody(rbInfo);

	physics->GetWorld()->addRigidBody(body);
}

void ClothPart::Update(double dTime)
{
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);

	btVector3 pos = trans.getOrigin();
	btQuaternion rotation = trans.getRotation();
	btVector3 rotationAxis = rotation.getAxis();

	x = pos.getX();
	y = pos.getY();
	z = pos.getZ();

	xRot = rotationAxis.getX();
	yRot = rotationAxis.getY();
	zRot = rotationAxis.getZ();
	rotationAngle = Utils::RadToDeg(rotation.getAngle());

	if (isDebugDraw)
	{
		sphere->Update(dTime);
	}

	if (isHeld)
	{
		glm::vec3 pos = camera->GetCamPos();
		glm::vec3 moveTo = pos - GetPosition();
		moveTo.y -= 0.0f;
		moveTo.z -= 50.0f;
		moveTo *= 1.0f;

		body->activate(true);
		body->setLinearVelocity(btVector3(moveTo.x, moveTo.y, moveTo.z));
	}

	//Dropping the object
	if (inputManager->GetMouseState(MOUSE_LEFT) == KEY_UP && isHeld == true)
	{
		isHeld = false;
		body->activate(true);
	}

	//Constraining object to level bounds
	if (x > 120.0f || x < -120.0f || z > 120.0f || z < -120.0f)
	{
		glm::vec3 moveTo = -GetPosition();
		glm::normalize(moveTo);
		moveTo *= 0.1f;

		body->setLinearVelocity(btVector3(moveTo.x, moveTo.y, moveTo.z));
	}
}

void ClothPart::Draw()
{
	float offset = 1.0f;

	if (isDebugDraw)
	{
		sphere->Draw();

		//if (col < cloth->getNumCols() - 1 && row < cloth->getNumRows() - 1)
		//{
		//	glFrontFace(GL_CW);

		//	//glBegin(GL_TRIANGLE_STRIP);
		//	//glScalef(4.0f, 4.0f, 4.0f);
		//	//glVertex3f(sphere->GetX(), sphere->GetY(), sphere->GetZ()); //vertex 2
		//	//glVertex3f(part2->sphere->GetX() + offset, part2->sphere->GetY(), part2->sphere->GetZ()); //vertex 4
		//	//glVertex3f(part1->sphere->GetX(), part1->sphere->GetY() + offset, part1->sphere->GetZ()); //vertex 1
		//	//glVertex3f(part3->sphere->GetX() + offset, part3->sphere->GetY() + offset, part3->sphere->GetZ() ); //vertex 3
	
		//	//glVertex3f(this->GetX(), this->GetY(), this->GetZ()); //vertex 2ss
		//	
		//	//glEnd();
		//}
	}

	
}

void ClothPart::MousePressing()
{
	if (!isStatic)
	{
		isHeld = true;
	}
}

void ClothPart::SetIsStatic(bool _static)
{
	isStatic = _static;
}

btRigidBody * ClothPart::GetBody()
{
	return body;
}

void ClothPart::SetParts()
{

	if (col < cloth->getNumCols() - 1 && row < cloth->getNumRows() - 1)
	{
		part1 = cloth->FindPart(row, col + 1);
		part2 = cloth->FindPart(row + 1, col);
		part3 = cloth->FindPart(row + 1, col + 1);
	}
}
