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
	isDebugDraw = false;
	isStatic = false;
	isEndPart = false;
	ambientStr = 0.01f;
	ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightPos = glm::vec3(-200.0f, -600.0f, -300.0f);

	isPart1Detached = false;
	isPart2Detached = false;
	isPart3Detached = false;

	overlap = 0.1f;
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
		mass = 0.05f;
		colShape->calculateLocalInertia(mass, localInertia);
	}

	startTransform.setOrigin(btVector3(x, y, z));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	body = new btRigidBody(rbInfo);

	physics->GetWorld()->addRigidBody(body);

	//Init quad
	float ambientFactor = float(col + 1 / cloth->getNumCols());
	ambientStr = 0.001f * ambientFactor;

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
		moveTo.y -= 10.0f;
		moveTo.z -= 40.0f;
		moveTo *= 2.0f;

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

	if (!isEndPart)
	{
		DrawQuad();
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
		isEndPart = false;
	}
	else
	{
		isEndPart = true;
	}

	//TexCoord Setup
	float texW = float(1.0f / cloth->getNumRows());
	float texH = -float(1.0f / cloth->getNumCols());
	float texCoordX = texW * row;
	float texCoordY = texH * col;
	float texCoordW = texW * (row + 1.0f);
	float texCoordH = texH * (col + 1.0f);

	//Part 0
	quadVerts[6] = texCoordX;
	quadVerts[7] = texCoordY;
	//Part 1
	quadVerts[14] = texCoordX;
	quadVerts[15] = texCoordH;
	//Part 2
	quadVerts[22] = texCoordW;
	quadVerts[23] = texCoordY;
	//Part 3
	quadVerts[30] = texCoordW;
	quadVerts[31] = texCoordH;
}

void ClothPart::InitQuad()
{
	texture = assetLoader->CreateTexture("Assets/clothTexture.png", "clothTexture");
	program = shaderLoader->CreateProgram("Assets/ClothVert.vs", "Assets/ClothFrag.fs", "clothVert", "clothFrag");

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

	if (!isEndPart)
	{
		UpdateVertices();
	}

	////Position
	glm::vec3 objPosition = glm::vec3(x, y, z);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), objPosition);

	//Rotation
	glm::vec3 rotationAxisZ = glm::vec3(0, 1, 0);
	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), rotationAxisZ);

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

	//glutPostRedisplay();
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

void ClothPart::UpdateVertices()
{
	//Find positions
	glm::vec3 part1Pos = part1->GetPosition() - GetPosition();
	glm::vec3 part2Pos = part2->GetPosition() - GetPosition();
	glm::vec3 part3Pos = part3->GetPosition() - GetPosition();

	//Check if any part has disconnected
	float detachDist = cloth->GetCellSpacing() * 4.5f;

	float part1Dist = glm::distance(part1->GetPosition(), GetPosition());
	float part2Dist = glm::distance(part2->GetPosition(), GetPosition());
	float part3Dist = glm::distance(part3->GetPosition(), GetPosition());

	if (part1Dist > detachDist) { isPart1Detached = true; }
	if (part2Dist > detachDist) { isPart2Detached = true; }
	if (part3Dist > detachDist) { isPart3Detached = true; }

	//Change the positions within the vertex array
	//Part 1
	if (!isPart1Detached)
	{
		quadVerts[8] = part1Pos.x;
		quadVerts[9] = part1Pos.y + overlap;
		quadVerts[10] = part1Pos.z;
	}
	else
	{
		quadVerts[8] = 0.0f;
		quadVerts[9] = 0.0f;
		quadVerts[10] = 0.0f;
	}

	//Part 2
	if (!isPart2Detached)
	{
		quadVerts[16] = part2Pos.x + overlap;
		quadVerts[17] = part2Pos.y;
		quadVerts[18] = part2Pos.z;
	}
	else
	{
		quadVerts[16] = 0.0f;
		quadVerts[17] = 0.0f;
		quadVerts[18] = 0.0f;
	}

	//Part 3
	if (!isPart3Detached)
	{
		quadVerts[24] = part3Pos.x + overlap;
		quadVerts[25] = part3Pos.y + overlap;
		quadVerts[26] = part3Pos.z + overlap;
	}
	else
	{
		quadVerts[24] = 0.0f;
		quadVerts[25] = 0.0f;
		quadVerts[26] = 0.0f;
	}

	//Updating the vertex array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
