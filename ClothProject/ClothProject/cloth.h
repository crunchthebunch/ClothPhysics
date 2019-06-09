#pragma once
#include "level.h"
#include "physicsmanager.h"
#include "clothpart.h"

//Cloth itself is not an object, but instead is a "manager" for a vector of cloth parts (spheres) that
//are joined together by constraints so that the cloth can collide and react like cloth.
//This class will also be used to generate and draw the mesh of the cloth.

class Cloth : public GameObject
{
public:
	Cloth(Level* _level, PhysicsManager* _physicsManager);
	virtual ~Cloth();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	int getNumRows();
	int getNumCols();
	void setNumCols(int value);
	void setNumRows(int value);

	std::vector<ClothPart*> getVecParts();

	float GetCellSpacing() const;

	void SetVecPickable(std::vector<GameObject*>* _vecPickable);
	ClothPart* FindPart(int _x, int _y);

private:
	void CreateSpring(btRigidBody* bodyA, btRigidBody* bodyB);
	void ConstrainParts();
	

private:
	PhysicsManager * physics;
	Level* level;
	std::vector<ClothPart*> vecParts;
	std::vector<btGeneric6DofConstraint*> vecSprings;
	std::vector<GameObject*>* vecPickable;
	std::vector<GLfloat> vecPartsVertices;

	float width;
	float height;
	int numRows;
	int numCols;

	Mesh* clothMesh;

	float cellSpacing;

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