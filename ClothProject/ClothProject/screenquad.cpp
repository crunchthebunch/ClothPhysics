#include "screenquad.h"

ScreenQuad::ScreenQuad(Level * _level, GLuint _FBO)
{
	this->level = _level;
	VBO = level->GetVBO();
	shaderLoader = level->GetShaderLoader();
	clock = level->GetClock();
	FBO = _FBO;
	screenResolution = glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
	timeMS = (GLfloat)0.0f;
	fxMode = 1;
}

ScreenQuad::~ScreenQuad()
{
	glDeleteTextures(1, &frameTexture);
}

void ScreenQuad::Initialise()
{
	program = shaderLoader->CreateProgram("Assets/frameVert.vs", "Assets/frameFrag.fs", "frameVert", "frameFrag");

	GLfloat vertices[] =
	{
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	//VAO
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Attributes
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Generate FrameTexture
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &frameTexture);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0); //Attach to bound FBO

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//FrameTexture location
	frameTextureLoc = glGetUniformLocation(program, "frameTexture");
	timeLoc = glGetUniformLocation(program, "timeMS");
	screenResolutionLoc = glGetUniformLocation(program, "resolution");
	fxModeLoc = glGetUniformLocation(program, "fxMode");

	glBindVertexArray(0);
}

void ScreenQuad::Update(double dTime)
{
	timeMS = (float)clock->GetMsPassed()*0.001f;
	glutPostRedisplay();
}

void ScreenQuad::Draw()
{
	glUseProgram(program);
	glDisable(GL_DEPTH_TEST);

	//Texture
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(frameTextureLoc, 0);
	glUniform1i(fxModeLoc, fxMode);
	glUniform1f(timeLoc, timeMS);
	glUniform2i(screenResolutionLoc, (GLint)screenResolution.x, (GLint)screenResolution.y);

	glBindTexture(GL_TEXTURE_2D, frameTexture);

	//Drawing
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ScreenQuad::SetFXMode(int _mode)
{
	fxMode = _mode;
}
