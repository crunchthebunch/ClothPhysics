#pragma once
#include "level.h"
#include "physicsmanager.h"
#include "meshsphere.h"
//#include "cloth.h"

class Cloth;
//Cloth Part is a rigidbody sphere collider that will be constrained to other cloth parts to create a collidable cloth

class ClothPart : public GameObject
{
public:
	ClothPart(Level* _level, PhysicsManager* _physicsManager, Cloth* cloth, int row, int col);
	virtual ~ClothPart();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	virtual void MousePressing();

	void SetIsStatic(bool _static);
	btRigidBody* GetBody();

	int row;
	int col;

	void SetParts();

	void InitQuad();
	void UpdateQuad(float dTime);
	void DrawQuad();

	void UpdateVertices();

	void ResetMesh();
	void ResetPosition();

private:
	PhysicsManager * physics;
	btRigidBody* body;
	Level* level;
	MeshSphere* sphere;
	bool isDebugDraw;
	bool isStatic;
	bool isHeld;
	bool isEndPart;

	float overlap;
	ClothPart* part1;
	ClothPart* part2;
	ClothPart* part3;

	bool isPart1Detached;
	bool isPart2Detached;
	bool isPart3Detached;

	Cloth* cloth;

	GLfloat quadVerts[32] =
	{
		//---------------------------------------------------- First Triangle (Left-bottom)
		// Position			 // Color			 //Tex Coords
		0.0f, 0.0f, 0.0f,	 1.0f, 1.0f, 1.0f,	 0.0f, 1.0f,	//Left
		1.0f, 0.0f, 0.0f,	 1.0f, 1.0f, 1.0f,	 1.0f, 1.0f,	//Right
		0.0f,  1.0f, 0.0f,	 1.0f, 1.0f, 1.0f,	 0.0f, 0.0f,	//Top
		//----------------------------------------------------- Second Triangle (Right-top)
		1.0f,  1.0f, 0.0f,	 1.0f, 1.0f, 1.0f,	 1.0f, 0.0f		//Left
	};

	GLuint quadIndices[6] =
	{
		0, 1, 2, //1st Triangle
		3, 2, 1  //2nd Triangle
	};

	GLuint VAO;
	GLuint EBO;
	GLuint program;

	GLuint modelLoc;
	GLuint transInvModelLoc;
	GLuint texLoc;
	GLuint ambientStrLoc;
	GLuint ambientColorLoc;

	GLuint lightColorLoc;
	GLuint lightPosLoc;
	GLuint camPosLoc;
	GLuint MVPLoc;
	GLuint skyBoxLoc;

	GLuint skyBoxTexture;

	float ambientStr;
	glm::vec3 ambientColor;
	glm::vec3 lightColor;
	glm::vec3 lightPos;
	glm::vec3 camPos;

	glm::vec3 fillColor;
	glm::vec2 texOffset;
	glm::vec2 texScale;
	glm::mat4 model;
	glm::mat4 transInvModel;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 MVP;
};