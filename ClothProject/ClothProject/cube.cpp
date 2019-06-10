#include "cube.h"
#include "cubemap.h"

Cube::Cube(Level * _level, PhysicsManager* _physicsManager, Terrain * _terrain)
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
	terrain = _terrain;
	physics = _physicsManager;
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;

	isHeld = false;
}

Cube::~Cube()
{
	delete model;
}

void Cube::Initialise()
{
	SetUniformScale(0.05f);

	model = new Model("Assets/CubeModel.obj", this);
	
	//Physics
	colShape = new btBoxShape(btVector3(3.0f, 3.0f, 3.0f));
	physics->GetCollisionShapes()->push_back(colShape);

	//btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(1.0f);

	btVector3 localInertia;
	colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(x, y, z));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia*2);

	body = new btRigidBody(rbInfo);

	physics->GetWorld()->addRigidBody(body);
}

void Cube::Update(double dTime)
{
	//body->applyTorque(btVector3(5.0f, 20.0f, 0.0f));

	btTransform trans;
	//body->getMotionState()->getWorldTransform(trans);
	trans = body->getInterpolationWorldTransform();

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

	if (isHeld)
	{
		glm::vec3 pos = camera->GetCamPos();
		glm::vec3 moveTo = pos - GetPosition();
		moveTo.y -= 5.0f;
		moveTo.z -= 20.0f;
		moveTo *= 5.0f;

		body->activate(true);
		body->setLinearVelocity(btVector3(moveTo.x, moveTo.y, moveTo.z));
	}

	//Dropping the object
	if (inputManager->GetMouseState(MOUSE_LEFT) == KEY_UP && isHeld == true)
	{
		isHeld = false;
		body->activate(true);
		body->applyCentralImpulse(btVector3(0.0f, 10.0f, -100.0f));
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

void Cube::Draw()
{
	//Render regular cube
	model->Draw();
}

void Cube::MousePressing()
{
	//Pick up cube and place in front of camera
	isHeld = true;
}

void Cube::Reset()
{
	body->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
	body->setWorldTransform(startTransform);
}
