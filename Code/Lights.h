#pragma once

#include "platform.h"


enum LightType
{
	LightType_Directional = 0,
	LightType_Point
};
struct Light
{
	LightType type;
	glm::vec3 color;
	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 intensity;
	bool enabled;
	u32 model;

	glm::mat4 GetTransformMat()
	{
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), direction.x, { 1.0f, 0.0f, 0.0f }) *
			glm::rotate(glm::mat4(1.0f), direction.y, { 0.0f, 1.0f, 0.0f }) *
			glm::rotate(glm::mat4(1.0f), direction.z, { 0.0f, 0.0f, 1.0f });

		if (type == LightType::LightType_Directional)
		{
			return glm::translate(glm::mat4(1.0f), position) *
				rot *
				glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		}
		else
		{
			return glm::translate(glm::mat4(1.0f), position) *
				rot *
				glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
		}
		
	}
};