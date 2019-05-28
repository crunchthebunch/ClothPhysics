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
	if (isDebugDraw)
	{
		sphere->Draw();
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
