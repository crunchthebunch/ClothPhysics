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
	isStatic = false;
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
}

void ClothPart::Draw()
{
	if (isDebugDraw)
	{
		sphere->Draw();
	}

	//glFrontFace(GL_CW);

	//glBegin(GL_TRIANGLE_STRIP);
	//glVertex3f(0.0f, 1.0f, 0.0f); //vertex 1
	//glVertex3f(0.0f, 0.0f, 0.0f); //vertex 2
	//glVertex3f(1.0f, 1.0f, 0.0f); //vertex 3
	//glVertex3f(1.5f, 0.0f, 0.0f); //vertex 4
	//glEnd();

}

void ClothPart::SetIsStatic(bool _static)
{
	isStatic = _static;
}

btRigidBody * ClothPart::GetBody()
{
	return body;
}
