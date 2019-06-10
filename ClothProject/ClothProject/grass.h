#pragma once
#include "gameobject.h"
#include "level.h"
#include "terrain.h"

class Grass : public GameObject
{
public:
	Grass(Level* _level, Terrain* _terrain);
	virtual ~Grass();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

private:
	void LoadVegetationMap();
	void BuildVertices();

private:
	Level * level;

	GLuint myVBO;
	GLuint VAO;
	GLuint EBO;
	GLuint program;

	GLuint MVPLoc;
	GLuint texLoc;

	Terrain* terrain;
	int vertexAmount;
	int numRows;
	int numCols;
	std::vector<TerrainVertex>* terrainVertices;
	std::vector<float> vegetationMap;
	float heightScale;
	float heightOffset;

	glm::mat4 MVP;
};