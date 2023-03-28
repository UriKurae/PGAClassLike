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
};