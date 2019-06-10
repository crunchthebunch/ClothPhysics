#include "mousepicker.h"
#include "utils.h"

MousePicker::MousePicker(Level * _level, std::vector<GameObject*>* _vecPickable)
{
	level = _level;
	vecPickable = _vecPickable;
	camera = level->GetCamera();
	inputManager = level->GetInputManager();
	radius = 1.5f;
}

MousePicker::~MousePicker()
{
}

void MousePicker::UpdateMousePicker()
{
	mouseX = ((2.0f * inputManager->GetMouseX()) / (float)WINDOW_WIDTH) - 1.0f;
	mouseY = 1.0f - ((2.0f * inputManager->GetMouseY()) / (float)WINDOW_HEIGHT);
	screenMousePos = glm::vec2(mouseX, mouseY);

	//Screen to Projection
	glm::vec4 clipCoords = glm::vec4(mouseX, mouseY, -1.0f, 1.0f);

	//Projection to Eye
	glm::mat4 invProjMat = glm::inverse(camera->GetProjection());
	glm::vec4 eyeCoords = invProjMat * clipCoords;
	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

	//Eye to World
	glm::mat4 invViewMat = glm::inverse(camera->GetView());
	glm::vec4 rayWorld = invViewMat * eyeCoords;
	rayDirection = glm::normalize(glm::vec3(rayWorld));
}

bool MousePicker::CheckMouseCollision()
{
	std::vector<GameObject*>::iterator it;

	for (it = vecPickable->begin(); it != vecPickable->end(); it++)
	{
		GameObject* obj = *it;

		glm::vec3 objPos = glm::vec3(obj->GetX(), obj->GetY(), obj->GetZ());
		glm::vec3 v = objPos - camera->GetCamPos();

		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(v, rayDirection);
		float c = glm::dot(v, v) - (radius * radius);

		float d = ((b * b) - (4.0f * a * c));

		if (d > 0)
		{
			float x1 = (-b - sqrt(d)) / (2.0f);
			float x2 = (-b + sqrt(d)) / (2.0f);

			if ((x1 >= 0 && x2 >= 0) || (x1 < 0 && x2 >= 0) || (x1 < 0 && x2 < 0)) //Intersects
			{
				picked = obj;
				return true;
			}
		}
	}

	return false;
}

glm::vec2 MousePicker::GetScreenMousePos() const
{
	return screenMousePos;
}

glm::vec3 MousePicker::GetRayDirection() const
{
	return rayDirection;
}

GameObject * MousePicker::GetPickedObject() const
{
	return picked;
}
