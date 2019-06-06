#pragma once
#include "mousepicker.h"
#include "cube.h"
#include "sphere.h"
#include "capsule.h"
#include "pyramid.h"
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

	void ControlCamera();

	void MousePicking();

private:
	PhysicsManager* physicsManager;

	bool isWireframe;
	bool isPostFX;

	Texter* text;

	Cube* cube;
	Sphere* sphere;
	Pyramid* pyramid;
	//Capsule* capsule;

	GLuint FBO;
	GLuint RBO;

	ScreenQuad* screenQuad;

	MousePicker* mousePicker;
	std::vector<GameObject*> vecPickable;
};