#pragma once
#include "level.h"
#include "model.h"
#include "terrain.h"

class Cube : public GameObject
{
public:
	Cube(Level* _level, Terrain * _terrain);
	virtual ~Cube();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void ProcessInput(double dTime);

private:
	Level* level;
	Model* model;
	Model* stencil;
	Terrain* terrain;
};