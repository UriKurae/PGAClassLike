#pragma once

#include "platform.h"

class EditorCamera
{
public:

	EditorCamera(int width, int height, float near, float far);
	~EditorCamera();

	void Update(Input& input, float dt);
	void UpdateFov(float newFov);
	void Recalculate(int w, int h);

	glm::mat4& GetView() { return view; }
	glm::mat4& GetProjection() { return projection; }
	glm::mat4 GetViewProjection() { return projection * view; }
	glm::vec3& GetPosition() { return cameraPos; }

	void SetCameraSpeed(float speed) { cameraSpeed = speed; }
	void SetCameraZoomSpeed(float speed) { zoomSpeed = speed; }
	void SetMouseSensitivity(float sens) { sensitivity = sens; }


private:

	void MousePan(Input& input, float dt);
	void MouseOrbit(Input& input, float dt);
	void MouseMovement(Input& input, float dt);
	void Movement(Input& input, float dt);
private:

	glm::vec3 cameraPos;
	
	float cameraSpeed = 10.0f;
	float cameraSprint = 1.0f;
	float cameraSpeedMultiplier = 3.0f;
	float sensitivity = 1.0f;
	float zoomSpeed = 40.0f;

	glm::vec3 camUp;
	glm::vec3 camFront;

	// Matrices
	glm::mat4 view;
	glm::mat4 projection;

	// Aspect Ratio
	int width;
	int height;

	// Near far planes
	float nearPlane;
	float farPlane;
};