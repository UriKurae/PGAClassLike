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
private:

	glm::vec3 cameraPos;
	
	float cameraSpeed = 2.5f;

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