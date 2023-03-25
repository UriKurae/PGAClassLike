#include "Camera.h"

EditorCamera::EditorCamera(int w, int h, float near, float far)
{
	// Setup camera position, front and up
	cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	camUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Get view and projection matrix 
	view = glm::lookAt(cameraPos, cameraPos + camFront, camUp);
	projection = glm::perspective(glm::radians(80.0f), float(w) / (float)h, near, far);

	width = w;
	height = h;
	nearPlane = near;
	farPlane = far;

}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::Update(Input input, float dt)
{

	if (input.keys[K_W] == ButtonState::BUTTON_PRESSED)
		cameraPos += cameraSpeed * camFront * dt;
	if (input.keys[K_S] == ButtonState::BUTTON_PRESSED)
		cameraPos -= cameraSpeed * camFront * dt;
	if (input.keys[K_A] == ButtonState::BUTTON_PRESSED)
		cameraPos -= glm::normalize(glm::cross(camFront, camUp)) * (cameraSpeed * dt);
	if (input.keys[K_D] == ButtonState::BUTTON_PRESSED)
		cameraPos += glm::normalize(glm::cross(camFront, camUp)) * (cameraSpeed * dt);

	view = glm::lookAt(cameraPos, cameraPos + camFront, camUp);
}

void EditorCamera::UpdateFov(float newFov)
{
	projection = glm::perspective(glm::radians(newFov), float(width) / (float)height, nearPlane, farPlane);
}
