#include "cube.h"
#include "cubemap.h"

Cube::Cube(Level * level, Terrain * _terrain)
{
	this->level = level;
	VBO = level->GetVBO();
	camera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetAssetLoader();
	skyBoxTexture = level->GetSkyBox()->GetTexture();
	vecTexts = level->GetVecTexts();
	clock = level->GetClock();
	terrain = _terrain;
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;
}

Cube::~Cube()
{
	delete model;
	delete stencil;
}

void Cube::Initialise()
{
	SetUniformScale(0.05f);

	model = new Model("Assets/CubeModel.obj", this);
	stencil = new  Model("Assets/StencilModel.obj", this);
}

void Cube::Update(double dTime)
{
	deltaTime = (float)dTime;
	model->Update(deltaTime);
	stencil->Update(deltaTime);

	ProcessInput(dTime);
}

void Cube::Draw()
{
	//Enable the stencil test
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//1st pass
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);

	//Render regular cube
	model->Draw();

	//2nd pass
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	//Render scaled up cube
	stencil->Draw();

	//Disable writing to stencil mask
	glStencilMask(0x00);
	glDisable(GL_STENCIL_TEST);
}

void Cube::ProcessInput(double dTime)
{
	float speed = 40.0f;

	if (inputManager->GetKeyState('w') == KEY_DOWN || inputManager->GetKeyState('W') == KEY_DOWN)
	{
		z -= speed * (float)dTime;
	}
	else if (inputManager->GetKeyState('s') == KEY_DOWN || inputManager->GetKeyState('S') == KEY_DOWN)
	{
		z += speed * (float)dTime;
	}

	if (inputManager->GetKeyState('a') == KEY_DOWN || inputManager->GetKeyState('A') == KEY_DOWN)
	{
		x -= speed * (float)dTime;
	}
	else if (inputManager->GetKeyState('d') == KEY_DOWN || inputManager->GetKeyState('D') == KEY_DOWN)
	{
		x += speed * (float)dTime;
	}

	x = Utils::Clamp(x, -250.0f, 250.0f);
	z = Utils::Clamp(z, -250.0f, 250.0f);

	//Snapping to height of the terrain
	y = terrain->GetHeight(x,z) + 2.0f;
}