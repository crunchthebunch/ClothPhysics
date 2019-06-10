#include "terrain.h"
#include "cubemap.h"
#include <fstream>
#include <sstream>

Terrain::Terrain(Level * _level, PhysicsManager* _physicsManager, const char * _heightMapPath, const char* _texPath, const char* _texName, int _quality)
{
	level = _level;
	camera = level->GetCamera();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetAssetLoader();
	skyBoxTexture = level->GetSkyBox()->GetTexture();
	clock = level->GetClock();
	VBO = level->GetVBO();
	physics = _physicsManager;
	heightMapPath = _heightMapPath;
	texturePath = _texPath;
	textureName = _texName;
	quality = _quality;
	numRows = 513;
	numCols = 513;
	heightScale = 0.015f;
	heightOffset = 0.0f;
	cellSpacing = 0.5f;
	ambientStr = 0.25f;
	ambientColor = glm::vec3(0.4f, 0.8f, 1.0f);
	lightColor = glm::vec3(1.0f, 1.0f, 0.8f);
	lightPos = glm::vec3(-200.0f, -600.0f, -300.0f);
}

Terrain::~Terrain()
{
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
}

void Terrain::Initialise()
{
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	//xScale = 1.0f;
	//yScale = 1.0f;
	//zScale = 1.0f;

	vertexAmount = numRows * numCols;
	faceAmount = (numRows - 1) * (numCols - 1) * 2;

	LoadHeightmap();
	Smooth();

	texture = assetLoader->CreateTexture(texturePath, textureName);
	program = shaderLoader->CreateProgram("Assets/TerrainVert.vs", "Assets/TerrainFrag.fs", "vertModel0", "fragModelRim0");

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	BuildVertices();
	BuildIndices();

	//Backface Culling
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

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

	//Initialise physics ground plane
	InitPhysics();
}

void Terrain::Update(double dTime)
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

void Terrain::Draw()
{
	glUseProgram(program);

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
	glDrawElements(GL_TRIANGLES, faceAmount*3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDisable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Terrain::InitPhysics()
{
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(500.), btScalar(50.), btScalar(500.)));

	physics->GetCollisionShapes()->push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0.0f, -48.0f, 0.0f));

	btScalar mass(0.0f);

	btVector3 localInertia(0.0f, 0.0f, 0.0f);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	physics->GetWorld()->addRigidBody(body);
}

void Terrain::BuildVertices()
{
	terrainVertices.resize(vertexAmount);

	float halfWidth = (numCols - 1)*cellSpacing*0.5f;
	float halfDepth = (numRows - 1)*cellSpacing*0.5f;

	float du = 1.0f / (numCols - 1);
	float dv = 1.0f / (numRows - 1);

	for (int i = 0; i < numRows; ++i)
	{
		float zz = halfDepth - (i * cellSpacing);
		for (int j = 0; j < numCols; ++j)
		{
			float xx = -halfWidth + (j * cellSpacing);

			float yy = heightMap[i*numCols + j];
			terrainVertices[i*numCols + j].pos = glm::vec3(xx, yy, zz);
			terrainVertices[i*numCols + j].normal = glm::vec3(0.0f, 1.0f, 0.0f);

			// Stretch texture over grid.
			terrainVertices[i*numCols + j].texC.x = j * du;
			terrainVertices[i*numCols + j].texC.y = i * dv;
		}
	}

	// Estimate normals for interior nodes using central difference.
	float invTwoDX = 1.0f / (2.0f*cellSpacing);
	float invTwoDZ = 1.0f / (2.0f*cellSpacing);
	for (int i = 2; i < numRows - 1; ++i)
	{
		for (int j = 2; j < numCols - 1; ++j)
		{
			float t = heightMap[(i - 1)*numCols + j];
			float b = heightMap[(i + 1)*numCols + j];
			float l = heightMap[i*numCols + j - 1];
			float r = heightMap[i*numCols + j + 1];

			glm::vec3 tanZ(0.0f, (t - b)*invTwoDZ, 1.0f);
			glm::vec3 tanX(1.0f, (r - l)*invTwoDX, 0.0f);

			glm::vec3 N;
			N = glm::cross(tanZ, tanX);
			N = glm::normalize(N);

			terrainVertices[i*numCols + j].normal = N;
		}
	}

	//Transfer terrainVertices information to 1D vertices vector
	std::vector<GLfloat> vertices(vertexAmount*8);

	int k = 0;
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			//Position
			vertices[k + 0] = terrainVertices[i*numCols + j].pos.x;
			vertices[k + 1] = terrainVertices[i*numCols + j].pos.y;
			vertices[k + 2] = terrainVertices[i*numCols + j].pos.z;

			//Normal
			vertices[k + 3] = terrainVertices[i*numCols + j].normal.x;
			vertices[k + 4] = terrainVertices[i*numCols + j].normal.y;
			vertices[k + 5] = terrainVertices[i*numCols + j].normal.z;

			//Texture Coordinates
			vertices[k + 6] = terrainVertices[i*numCols + j].texC.x;
			vertices[k + 7] = terrainVertices[i*numCols + j].texC.y;

			k += 8;
		}
	}

	//Vertex Attributes
	glBufferData(GL_ARRAY_BUFFER, vertexAmount * 8 * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void Terrain::BuildIndices()
{
	std::vector<GLuint> indices(faceAmount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	int k = 0;
	for (int i = 0; i < numRows - 1; ++i)
	{
		for (int j = 0; j < numCols - 1; ++j)
		{
			indices[k] = i * numCols + j;
			indices[k + 1] = i * numCols + j + 1;
			indices[k + 2] = (i + 1)*numCols + j;

			indices[k + 3] = (i + 1)*numCols + j;
			indices[k + 4] = i * numCols + j + 1;
			indices[k + 5] = (i + 1)*numCols + j + 1;

			k += 6; // next quad
		}
	}

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * faceAmount * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Terrain::LoadHeightmap()
{
	// A height for each vertex
	std::vector<unsigned char> in(numRows *numCols);

	// Open the file.
	std::ifstream inFile;
	inFile.open(heightMapPath, std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array, and scale and offset the heights.
	heightMap.resize(numRows * numCols, 0);
	for (int i = 0; i < numRows * numCols; ++i)
	{
		heightMap[i] = (float)in[i] * heightScale + heightOffset;
	}
}

void Terrain::Smooth()
{
	std::vector<float> dest(heightMap.size());

	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			dest[i*numCols + j] = Average(i,j);
		}
	}

	// Replace the old heightmap with the filtered one.
	heightMap = dest;
}

bool Terrain::InBounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	if (i >= 0 && i < numRows && j >= 0 && j < numCols)
	{
		return true;
	}
	return false;
}

float Terrain::Average(int i, int j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += heightMap[m*numCols + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

float Terrain::GetWidth() const
{
	return (numCols - 1)*cellSpacing;
}

float Terrain::GetDepth() const
{
	return (numRows - 1)*cellSpacing;
}

float Terrain::GetHeight(float _x, float _z) const
{
	// Transform from terrain local space to "cell" space.
	float c = (_x + 0.5f*GetWidth()) / cellSpacing;
	float d = (_z - 0.5f*GetDepth()) / -cellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = heightMap[row*numCols + col];
	float B = heightMap[row*numCols + col + 1];
	float C = heightMap[(row + 1)*numCols + col];
	float D = heightMap[(row + 1)*numCols + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}

	return 0.0f;
}

std::vector<TerrainVertex>* Terrain::GetTerrainVertices()
{
	return &terrainVertices;
}

std::vector<float>* Terrain::GetHeightMap()
{
	return &heightMap;
}

int Terrain::GetVertexAmount() const
{
	return vertexAmount;
}

int Terrain::GetNumRows() const
{
	return numRows;
}

int Terrain::GetNumCols() const
{
	return numCols;
}
