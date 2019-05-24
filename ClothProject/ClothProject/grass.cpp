#include "grass.h"
#include "cubemap.h"
#include <fstream>
#include <sstream>

Grass::Grass(Level * _level, Terrain* _terrain)
{
	this->level = _level;
	VBO = level->GetVBO();
	camera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetAssetLoader();
	skyBoxTexture = level->GetSkyBox()->GetTexture();
	terrain = _terrain;
	terrainVertices = terrain->GetTerrainVertices();
	numRows = terrain->GetNumRows();
	numCols = terrain->GetNumCols();
	vertexAmount = terrain->GetVertexAmount();
	heightScale = 0.1f;
	heightOffset = 0.0f;
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	xScale = 1.0f;
	yScale = 1.0f;
	zScale = 1.0f;
	rotationAngle = 0.0f;
}

Grass::~Grass()
{
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
}

void Grass::Initialise()
{
	texture = assetLoader->CreateTexture("Assets/Grass.png", "grassTexture");
	program = shaderLoader->CreateGeoProgram("Assets/GrassVert.vs", "Assets/GrassFrag.fs", "Assets/GrassGeo.gs", "grassVert", "grassFrag", "grassGeo");

	MVPLoc = glGetUniformLocation(program, "mvp");
	texLoc = glGetUniformLocation(program, "tex");

	//Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	//VAO
	glGenBuffers(1, &myVBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);

	LoadVegetationMap();
	BuildVertices();

	glBindVertexArray(0);
}

void Grass::Update(double dTime)
{
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
	glm::mat4 model = translationMatrix * rotationZ * scaleMatrix;

	////Transposed, Inversed Model Matrix
	glm::mat4 transInvModel = glm::transpose(glm::inverse(model));

	////View Matrix
	glm::mat4 view = camera->GetView();

	////Projection Matrix
	glm::mat4 proj = camera->GetProjection();

	////MVP Matrix
	MVP = proj * view *model;

	glutPostRedisplay();
}

void Grass::Draw()
{
	glUseProgram(program);
	glDisable(GL_CULL_FACE);

	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));

	//Depth Enable
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Alpha Blending
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_ALPHA_TEST);

	//Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 512 * 512);
	glBindVertexArray(0);

	glDisable(GL_BLEND);
}

void Grass::LoadVegetationMap()
{
	// A height for each vertex
	std::vector<unsigned char> in(numRows *numCols);

	// Open the file.
	std::ifstream inFile;
	inFile.open("Assets/vegetationMap.raw", std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array, and scale and offset the heights.
	vegetationMap.resize(numRows * numCols, 0);
	for (int i = 0; i < numRows * numCols; ++i)
	{
		vegetationMap[i] = (float)in[i] * heightScale + heightOffset;
	}
}

void Grass::BuildVertices()
{
	std::vector<GLfloat> vertices(vertexAmount * 6);

	int k = 0;
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			TerrainVertex temp = terrainVertices->at(i*numCols + j);
			float vegeHeight = vegetationMap[i*numCols + j];

			//Position
			vertices[k + 0] = temp.pos.x;
			vertices[k + 1] = temp.pos.y;
			vertices[k + 2] = temp.pos.z;

			if (vegeHeight < 4.0f)
			{
				//Color
				vertices[k + 3] = 0.0f;
				vertices[k + 4] = 2.0f;
				vertices[k + 5] = 0.0f;
			}
			else if (vegeHeight >= 4.0f && vegeHeight < 8.0f)
			{
				//Color
				vertices[k + 3] = 1.0f;
				vertices[k + 4] = 1.0f;
				vertices[k + 5] = 0.0f;
			}
			else

			{
				//Color
				vertices[k + 3] = 1.0f;
				vertices[k + 4] = 1.0f;
				vertices[k + 5] = 0.0f;
			}

			k += 6;
		}
	}

	//Vertex Attributes
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexAmount * 6 * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);
}
