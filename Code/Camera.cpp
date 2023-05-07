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

	yaw = -90.0f;
	pitch = 0.0f;

}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::Update(Input& input, float dt)
{
	ELOG("Mouse delta %f %f", input.mouseDelta.x, input.mouseDelta.y);

	if (input.keys[K_LEFT_ALT] == ButtonState::BUTTON_PRESSED && input.mouseButtons[MouseButton::LEFT] == BUTTON_PRESSED)
	{
		float offsetX = input.mouseDelta.x * sensitivity;
		float offsetY = input.mouseDelta.y * sensitivity;
		
		yaw += offsetX;
		pitch += -offsetY;

		glm::vec3 angles = glm::vec3(offsetY, offsetX, 0.0f);
		glm::quat rotation = glm::quat(angles);

		
		camFront = glm::normalize(rotation * camFront);
	}

	if (input.keys[K_LEFT_CTRL] == ButtonState::BUTTON_PRESSED && input.mouseButtons[MouseButton::LEFT] == BUTTON_PRESSED)
	{
		float offsetX = input.mouseDelta.x;
		float offsetY = input.mouseDelta.y;


		cameraPos += glm::normalize(glm::cross(camFront, camUp)) * (offsetX* dt);
		cameraPos += glm::normalize(camUp) * (offsetY* dt);
	}
	
	if (input.keys[K_LEFT_SHIFT] == ButtonState::BUTTON_PRESSED)
	{
		cameraSprint = cameraSpeedMultiplier;
	}
	else
	{
		cameraSprint = 1.0f;
	}

	if (input.keys[K_W] == ButtonState::BUTTON_PRESSED)
		cameraPos += cameraSpeed * camFront * cameraSprint * dt;
	if (input.keys[K_S] == ButtonState::BUTTON_PRESSED)
		cameraPos -= cameraSpeed * camFront * cameraSprint * dt;
	if (input.keys[K_A] == ButtonState::BUTTON_PRESSED)
		cameraPos -= glm::normalize(glm::cross(camFront, camUp)) * (cameraSpeed * cameraSprint * dt);
	if (input.keys[K_D] == ButtonState::BUTTON_PRESSED)
		cameraPos += glm::normalize(glm::cross(camFront, camUp)) * (cameraSpeed * cameraSprint * dt);

	view = glm::lookAt(cameraPos, cameraPos + camFront, camUp);
}

void EditorCamera::UpdateFov(float newFov)
{
	projection = glm::perspective(glm::radians(newFov), float(width) / (float)height, nearPlane, farPlane);
}

void EditorCamera::Recalculate(int w, int h)
{
	// Setup camera position, front and up
	cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	camUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Get view and projection matrix 
	view = glm::lookAt(cameraPos, cameraPos + camFront, camUp);
	projection = glm::perspective(glm::radians(80.0f), float(w) / (float)h, nearPlane, farPlane);

	width = w;
	height = h;
}
