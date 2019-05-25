#pragma once
#include "mousepicker.h"
#include "cube.h"
#include "screenquad.h"

class PhysicsManager;

class PhysicsLevel : public Level
{
public:
	PhysicsLevel();
	virtual ~PhysicsLevel();

	virtual void Initialise(Game* _myGame, ShaderLoader* _shaderloader, AssetLoader* _textureLoader, InputManager* _inputManager);
	virtual void Update();
	virtual void Draw();

	virtual void ControlCamera();

private:
	PhysicsManager* physicsManager;

	bool isWireframe;
	bool isPostFX;

	Texter* text;

	Cube* cube;

	GLuint FBO;
	GLuint RBO;

	ScreenQuad* screenQuad;

	MousePicker* mousePicker;
	std::vector<GameObject*> vecPickable;
};