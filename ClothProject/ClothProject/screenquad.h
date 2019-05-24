#pragma once
#include "gameobject.h"
#include "level.h"

class ScreenQuad : public GameObject
{
public:
	ScreenQuad(Level* _level, GLuint _FBO);
	virtual ~ScreenQuad();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void SetFXMode(int _mode); //0 = normal, 1 = blur, chromatic, scanline

private:
	Level * level;

	glm::vec2 screenResolution;
	float timeMS;
	int fxMode;

	GLuint VAO;
	GLuint EBO;
	GLuint FBO;

	GLuint program;
	GLuint frameTexture;
	GLuint frameTextureLoc;
	GLuint screenResolutionLoc;
	GLuint timeLoc;
	GLuint fxModeLoc;
};