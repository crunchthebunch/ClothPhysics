#pragma once
#include "gameobject.h"
#include "level.h"

class Star : public GameObject
{
public:
	Star(Level* _level);
	virtual ~Star();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

private:
	Level* level;

	GLuint VAO;
	GLuint EBO;
	GLuint program;

	GLuint MVPLoc;
	glm::mat4 MVP;
};