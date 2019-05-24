#pragma once
#include "level.h"
#include "cube.h"

class MousePicker
{
public:
	MousePicker(Level* _level, std::vector<GameObject*>* _vecPickable);
	~MousePicker();

	void UpdateMousePicker();
	bool CheckMouseCollision();
	glm::vec2 GetScreenMousePos() const;
	glm::vec3 GetRayDirection() const;
	GameObject* GetPickedObject() const;

private:
	float mouseX;
	float mouseY;
	float radius;
	glm::vec2 screenMousePos;
	glm::vec3 rayDirection;

	InputManager* inputManager;
	Level* level;
	Camera* camera;
	GameObject* picked;

	std::vector<GameObject*>* vecPickable; //Pointer to a vector of pickable gameobjects
};