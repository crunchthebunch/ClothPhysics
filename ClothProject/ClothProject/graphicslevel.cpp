#include "graphicslevel.h"
#include "sprite.h"
#include "cubemap.h"
#include "model.h"
#include "terrain.h"
#include "grass.h"

GraphicsLevel::GraphicsLevel()
{
	isInit = false;
	isWireframe = false;
	isPostFX = false;
}

GraphicsLevel::~GraphicsLevel()
{
	while (vecObjects.size() > 0)
	{
		GameObject* temp = vecObjects[vecObjects.size() - 1];
		vecObjects.pop_back();
		delete temp;
	}

	while (vecTexts.size() > 0)
	{
		Texter* temp = vecTexts[vecTexts.size() - 1];
		vecTexts.pop_back();
		delete temp;
	}

	delete skyBox;
	delete clock;
	delete levelCamera;
	delete mousePicker;

	//Framebuffer
	glDeleteFramebuffers(1, &FBO);
	delete screenQuad;
}

void GraphicsLevel::Initialise(Game * _myGame, ShaderLoader * _shaderloader, AssetLoader * _assetLoader, InputManager * _inputManager)
{
	shaderLoader = _shaderloader;
	assetLoader = _assetLoader;
	inputManager = _inputManager;

	//VBO
	glEnable(GL_TEXTURE_2D);
	glGenBuffers(1, &levelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, levelVBO);

	//FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	
	//RBO for depth and stencil testing
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); //Attach RBO to depth & stencil attachment of bound FBO
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Clear Screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Init Camera
	levelCamera = new Camera();
	levelCamera->SetCamPos(glm::vec3(0.0f,25.0f,50.0f));

	//Init Mouse Picker
	mousePicker = new MousePicker(this, &vecPickable);

	//Init Clock
	clock = new Clock();
	clock->Initialise();

	//Init ScreenQuad
	screenQuad = new ScreenQuad(this, FBO);
	screenQuad->Initialise();
	screenQuad->SetFXMode(0);

	//Init Text
	text = new Texter("", "Assets/Fonts/arial.ttf", glm::vec3(-200.0f, -400.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	text->SetColor(glm::vec3(0.8f, 0.9f, 1.0f));
	vecTexts.push_back(text);

	Texter* text2 = new Texter("", "Assets/Fonts/arial.ttf", glm::vec3(-730.0f, 400.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	text2->SetColor(glm::vec3(0.8f, 0.0f, 0.6f));
	vecTexts.push_back(text2);

	//Init Sky Box Cubemap
	std::vector<std::string> vecCubeMapPaths;
	vecCubeMapPaths.push_back("right.jpg");
	vecCubeMapPaths.push_back("left.jpg");
	vecCubeMapPaths.push_back("top.jpg");
	vecCubeMapPaths.push_back("bottom.jpg");
	vecCubeMapPaths.push_back("back.jpg");
	vecCubeMapPaths.push_back("front.jpg");
	skyBox = new CubeMap(this, &vecCubeMapPaths, "Assets/CubeMap/");
	skyBox->Initialise();

	//Init Terrain
	Terrain* terrain = new Terrain(this,"Assets/mountainHeightMap.raw","Assets/mountainTexture.png","mountainTerrain",1);
	terrain->SetX(0.0f);
	terrain->SetY(0.0f);
	terrain->SetZ(0.0f);
	terrain->Initialise();
	vecObjects.push_back(terrain);

	//Init Grass
	Grass* grass = new Grass(this, terrain);
	grass->Initialise();
	vecObjects.push_back(grass);

	isInit = true;
}

void GraphicsLevel::Update()
{
	//Update time
	clock->Process();

	float deltaTime = (float)clock->GetDeltaTick();

	//Camera
	ControlCamera(); //Move the camera using WASD

	//Compute Views
	levelCamera->ComputeView();
	levelCamera->ComputeProjection();

	//Update cube map
	skyBox->Update(deltaTime);

	//Update objects
	for (unsigned int i = 0; i < vecObjects.size(); i++)
	{
		if (vecObjects[i]->GetIsActive())
		{
			vecObjects[i]->Update(deltaTime);
		}
	}

	//Update ScreenQuad
	screenQuad->Update(deltaTime);

	//Update Mouse Picker
	mousePicker->UpdateMousePicker();

	if (mousePicker->CheckMouseCollision())
	{
		if (mousePicker->GetPickedObject() == cube)
		{
			text->SetText("Cube");
		}
	}
	else
	{
		text->SetText("");
	}

	static bool wToggle = true;
	static float wToggleDelay = 0.0f;

	//Toggle WireFrame
	if ((inputManager->GetKeyState('t') == KEY_DOWN || inputManager->GetKeyState('T') == KEY_DOWN) && wToggle == true)
	{
		if (isWireframe == false)
		{
			isWireframe = true;
		}
		else
		{
			isWireframe = false;
		}
		wToggle = false;
	}

	if (wToggle == false)
	{
		wToggleDelay += 1.0f * (float)clock->GetDeltaTick();
		if (wToggleDelay >= 0.2f)
		{
			wToggle = true;
			wToggleDelay = 0.0f;
		}
	}

	static bool fToggle = true;
	static float fToggleDelay = 0.0f;

	//Toggle Post-FX
	if ((inputManager->GetKeyState('f') == KEY_DOWN || inputManager->GetKeyState('F') == KEY_DOWN) && fToggle == true)
	{
		if (isPostFX == false)
		{
			//screenQuad->SetFXMode(1);
			//isPostFX = true;
		}
		else
		{
			//screenQuad->SetFXMode(0);
			//isPostFX = false;
		}
		fToggle = false;
	}

	if (fToggle == false)
	{
		fToggleDelay += 1.0f * (float)clock->GetDeltaTick();
		if (fToggleDelay >= 0.2f)
		{
			fToggle = true;
			fToggleDelay = 0.0f;
		}
	}

	glutPostRedisplay();
}

void GraphicsLevel::Draw()
{
	//Bind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.55f, 0.25f, 0.55f, 1.0f); //clear

	//Draw cubemap
	skyBox->Draw();

	//Enabling wireframe view
	if (isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//Enable Backface-culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	//Draw objects
	for (unsigned int i = 0; i < vecObjects.size(); i++)
	{
		if (vecObjects[i]->GetIsActive())
		{
			vecObjects[i]->Draw();
		}
	}

	//Disable Backface-culling
	glDisable(GL_CULL_FACE);

	//Reset to filled view
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Draw texts
	for (unsigned int i = 0; i < vecTexts.size(); i++)
	{
		vecTexts[i]->Render();
	}

	//Bind default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.55f, 0.25f, 0.55f, 1.0f); //Clear Color
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw the screen quad
	screenQuad->Draw();

	glutSwapBuffers();
}

void GraphicsLevel::ControlCamera()
{
	float deltaTime = (float)clock->GetDeltaTick();
	float camSpeed = 80.0f;

	glm::vec3 camMovement = glm::vec3(0.0f, 0.0f, 0.0f);

	if (inputManager->GetArrowState(DIR_UP) == KEY_DOWN)
	{
		camMovement.z -= camSpeed * deltaTime;
	}
	else if (inputManager->GetArrowState(DIR_DOWN) == KEY_DOWN)
	{
		camMovement.z += camSpeed * deltaTime;
	}

	if (inputManager->GetArrowState(DIR_RIGHT) == KEY_DOWN)
	{
		camMovement.x += camSpeed * deltaTime;
	}
	else if (inputManager->GetArrowState(DIR_LEFT) == KEY_DOWN)
	{
		camMovement.x -= camSpeed * deltaTime;
	}

	levelCamera->SetCamPos(levelCamera->GetCamPos() + camMovement);
}
