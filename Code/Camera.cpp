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

void EditorCamera::Update(Input& input, float dt)
{
	MouseMovement(input, dt);

	MousePan(input, dt);

	Movement(input, dt);

	MouseOrbit(input, dt);

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

void EditorCamera::MousePan(Input& input, float dt)
{
	if (input.keys[K_LEFT_ALT] == ButtonState::BUTTON_PRESSED && input.mouseButtons[MouseButton::MIDDLE] == BUTTON_PRESSED)
	{
		float offsetX = -input.mouseDelta.x;
		float offsetY = input.mouseDelta.y;


		cameraPos += glm::normalize(glm::cross(camFront, camUp)) * (offsetX * dt);
		cameraPos += glm::normalize(camUp) * (offsetY * dt);
	}
}

void EditorCamera::MouseOrbit(Input& input, float dt)
{
	if (input.keys[K_LEFT_ALT] == ButtonState::BUTTON_PRESSED && input.mouseButtons[MouseButton::LEFT] == BUTTON_PRESSED)
	{
		float offsetX = -input.mouseDelta.x * dt;
		float offsetY = -input.mouseDelta.y * dt;

		glm::vec3 target = glm::vec3(0.0f);

		glm::vec3 direction = cameraPos - target;
		
		glm::vec3 euler;
		
		if (cameraPos.z >= 0.0f)
			euler = glm::vec3(offsetY, offsetX, 0.0f);
		else if (cameraPos.z < 0.0f)
			euler = glm::vec3(-offsetY, offsetX, 0.0f);

		glm::quat rotate = glm::quat(euler);
		
		cameraPos = glm::normalize(rotate) * cameraPos;
		
		camFront = glm::normalize(target - cameraPos);
		
	}
}


void EditorCamera::MouseMovement(Input& input, float dt)
{
	if (input.mouseButtons[MouseButton::RIGHT] == BUTTON_PRESSED)
	{
		float offsetX = -input.mouseDelta.x * sensitivity * dt;
		float offsetY = -input.mouseDelta.y * sensitivity * dt;

		glm::vec3 angles = glm::vec3(offsetY, offsetX, 0.0f);
		glm::quat rotation = glm::quat(angles);


		camFront = glm::normalize(rotation * camFront);
	}

	if (input.mouseButtons[MouseButton::SCROLL] == BUTTON_SCROLL_UP)
	{
		cameraPos += zoomSpeed * camFront * cameraSprint * dt;
	}
	if (input.mouseButtons[MouseButton::SCROLL] == BUTTON_SCROLL_DOWN)
	{
		cameraPos -= zoomSpeed * camFront * cameraSprint * dt;
	}
}

void EditorCamera::Movement(Input& input, float dt)
{
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
	if (input.keys[K_E] == ButtonState::BUTTON_PRESSED)
		cameraPos += glm::normalize(camUp) * (cameraSpeed * dt);
	if (input.keys[K_Q] == ButtonState::BUTTON_PRESSED)
		cameraPos -= glm::normalize(camUp) * (cameraSpeed * dt);

}
