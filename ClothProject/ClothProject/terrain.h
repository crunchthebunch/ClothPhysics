#pragma once
#include "sprite.h"
#include "level.h"
#include "physicsmanager.h"

struct TerrainVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texC;
};

class Terrain : public GameObject
{
public:
	Terrain(Level* _level, PhysicsManager* _physicsManager, const char* _heightMap, const char* _texPath, const char* _texName, int _quality);
	virtual ~Terrain();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	float GetWidth() const;								//Get the X scale of the terrain
	float GetDepth() const;								//Get the Z scale of the terrain
	float GetHeight(float _x, float _z) const;			//Get the Y scale of the terrain

	std::vector<TerrainVertex>* GetTerrainVertices();	//Get terrain vertex vector
	std::vector<float>* GetHeightMap();					//Get terrain heightmap
	int GetVertexAmount() const;
	int GetNumRows() const;
	int GetNumCols() const;

private:
	void InitPhysics();
	void BuildVertices();
	void BuildIndices();
	void LoadHeightmap();
	void Smooth();
	bool InBounds(int i, int j);
	float Average(int i, int j);

private:
	Level * level;
	Sprite* sprite;
	const char* heightMapPath;
	const char* texturePath;
	const char* textureName;
	std::vector<float> heightMap;
	int vertexAmount;
	int faceAmount;
	int quality;

	std::vector<TerrainVertex> terrainVertices;

	int numRows;
	int numCols;
	float cellSpacing;
	float heightScale;
	float heightOffset;

	PhysicsManager* physics;

private:
	GLuint VAO;
	GLuint EBO;
	GLuint program;
	GLuint modelLoc;
	GLuint transInvModelLoc;
	GLuint texLoc;
	GLuint ambientStrLoc;
	GLuint ambientColorLoc;
	GLuint lightColorLoc;
	GLuint lightPosLoc;
	GLuint camPosLoc;
	GLuint MVPLoc;
	GLuint skyBoxLoc;
	GLuint skyBoxTexture;

	float ambientStr;
	glm::vec3 ambientColor;
	glm::vec3 lightColor;
	glm::vec3 lightPos;
	glm::vec3 camPos;

	glm::vec3 fillColor;
	glm::vec2 texOffset;
	glm::vec2 texScale;
	glm::mat4 model;
	glm::mat4 transInvModel;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 MVP;
};