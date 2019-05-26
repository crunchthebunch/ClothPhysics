#include "clothpart.h"
#include "cubemap.h"

ClothPart::ClothPart(Level * _level, PhysicsManager * _physicsManager)
{
	this->level = _level;
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

	btScalar mass(0.1f);

	btVector3 localInertia;
	colShape->calculateLocalInertia(mass, localInertia);

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
}

void ClothPart::Draw()
{
	if (isDebugDraw)
	{
		sphere->Draw();
	}
}

btRigidBody * ClothPart::GetBody()
{
	return body;
}
