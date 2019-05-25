#include "cube.h"
#include "cubemap.h"

Cube::Cube(Level * level, PhysicsManager* _physicsManager, Terrain * _terrain)
{
	this->level = level;
	VBO = level->GetVBO();
	camera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetAssetLoader();
	skyBoxTexture = level->GetSkyBox()->GetTexture();
	vecTexts = level->GetVecTexts();
	clock = level->GetClock();
	terrain = _terrain;
	physics = _physicsManager;
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;
}

Cube::~Cube()
{
	delete model;
	delete stencil;
}

void Cube::Initialise()
{
	SetUniformScale(0.05f);

	model = new Model("Assets/CubeModel.obj", this);
	stencil = new  Model("Assets/StencilModel.obj", this);
	
	//Physics
	colShape = new btBoxShape(btVector3(4.0f, 4.0f, 4.0f));
	physics->GetCollisionShapes()->push_back(colShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(1.0f);

	bool isDynamic = (mass != 0.0f);

	btVector3 localInertia;
	colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(0.0f, 50.0f, 0.0f));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	body = new btRigidBody(rbInfo);

	physics->GetWorld()->addRigidBody(body);
}

void Cube::Update(double dTime)
{
	body->applyTorque(btVector3(5.0f, 20.0f, 0.0f));
	//body->applyCentralForce(btVector3(20.0f, 0.0f, 0.0f));

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

	deltaTime = (float)dTime;
	model->Update(deltaTime);
	stencil->Update(deltaTime);
}

void Cube::Draw()
{
	//Enable the stencil test
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//1st pass
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);

	//Render regular cube
	model->Draw();

	//2nd pass
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	//Render scaled up cube
	stencil->Draw();

	//Disable writing to stencil mask
	glStencilMask(0x00);
	glDisable(GL_STENCIL_TEST);
}