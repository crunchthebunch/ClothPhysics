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
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
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

	//Init quad
	InitQuad();
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

	UpdateQuad((float)dTime);

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
		//sphere->Draw();
	}

	DrawQuad();
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

void ClothPart::InitQuad()
{
	texture = assetLoader->CreateTexture("Assets/cube.png", "cube");
	program = shaderLoader->CreateProgram("Assets/VertexShader3D.vs", "Assets/FragmentShaderRimModel.fs", "vertShader3D0", "fragShaderRimModel0");

	//VBO
	glEnable(GL_TEXTURE_2D);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices, GL_STATIC_DRAW);

	//Vertex Attributes
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//Backface Culling
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);

	//Get Uniform Locations
	modelLoc = glGetUniformLocation(program, "model");
	transInvModelLoc = glGetUniformLocation(program, "transInvModel");
	MVPLoc = glGetUniformLocation(program, "MVP");
	texLoc = glGetUniformLocation(program, "texture_diffuse1");
	skyBoxLoc = glGetUniformLocation(program, "skybox");

	ambientStrLoc = glGetUniformLocation(program, "ambientStr");
	ambientColorLoc = glGetUniformLocation(program, "ambientColor");
	lightColorLoc = glGetUniformLocation(program, "lightColor");
	lightPosLoc = glGetUniformLocation(program, "lightPos");
	camPosLoc = glGetUniformLocation(program, "camPos");
}

void ClothPart::UpdateQuad(float dTime)
{
	deltaTime = (float)dTime;

	////Position
	glm::vec3 objPosition = glm::vec3(x, y, z);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), objPosition);

	//Rotation
	glm::vec3 rotationAxisZ = glm::vec3(xRot, yRot, zRot);
	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxisZ);

	//Scale
	glm::vec3 objScale = glm::vec3(xScale, yScale, zScale);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), objScale);

	////Model Matrix
	model = translationMatrix * rotationZ * scaleMatrix;

	////Transposed, Inversed Model Matrix
	transInvModel = glm::transpose(glm::inverse(model));

	////View Matrix
	view = camera->GetView();

	////Projection Matrix
	proj = camera->GetProjection();

	////MVP Matrix
	MVP = proj * view * model;

	//Texture Coordinate Offset & Scale
	camPos = camera->GetCamPos();

	glutPostRedisplay();
}

void ClothPart::DrawQuad()
{
	glUseProgram(program);

	//Alpha Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Depth Enable
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Model Matrix
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//Trans Inv Model
	glUniformMatrix4fv(transInvModelLoc, 1, GL_FALSE, glm::value_ptr(transInvModel));

	//MVP Matrix
	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));

	//Lighting
	glUniform1f(ambientStrLoc, ambientStr);
	glUniform3f(ambientColorLoc, ambientColor.r, ambientColor.g, ambientColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(camPosLoc, camPos.x, camPos.y, camPos.z);

	//Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	//Skybox
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(skyBoxLoc, 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Drawing based on verts
	glBindVertexArray(VAO);
	//glPolygonMode(GL_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDisable(GL_BLEND);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
