#pragma once
#include "mousepicker.h"
#include "cube.h"
#include "sphere.h"
#include "capsule.h"
#include "pyramid.h"
#include "cloth.h"
#include "clothpart.h"
#include "screenquad.h"

class PhysicsManager;
class Cloth;
class ClothPart;

class PhysicsLevel : public Level
{
public:
	PhysicsLevel();
	virtual ~PhysicsLevel();

	virtual void Initialise(Game* _myGame, ShaderLoader* _shaderloader, AssetLoader* _textureLoader, InputManager* _inputManager);
	virtual void Update();
	virtual void Draw();

	void ControlCamera();

	void ProcessInput();
	void MousePicking();

	void ResetLevel();

	bool buttonPressed;

private:
	PhysicsManager* physicsManager;

	bool isWireframe;
	bool isPostFX;
	bool isHolding;

	Texter* text;
	Texter* lengthChange;
	Texter* widthChange;

	Cube* cube;
	Sphere* sphere;
	Pyramid* pyramid;
	Capsule* capsule;
	Cloth* cloth;

	GLuint FBO;
	GLuint RBO;

	ScreenQuad* screenQuad;

	MousePicker* mousePicker;
	std::vector<GameObject*> vecPickable;
};