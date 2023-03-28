#pragma once

#include "platform.h"

// ------ VBO ------
struct VertexBufferAttribute
{
	u8 location;
	u8 componentCount;
	u8 offset;
};

struct VertexBufferLayout
{
	std::vector<VertexBufferAttribute> attributes;
	u8 stride;
};

// ------ VBO End ------

// ------ Shader ------

struct VertexShaderAttribute
{
	u8 location;
	u8 componentCount;
};

struct VertexShaderLayout
{
	std::vector<VertexShaderAttribute> attributes;
};

// ------ Shader End ------

// ------ VAO ------

struct Vao
{
	u32 handle;
	u32 programHandle;
};

// ------ VAO End ------

