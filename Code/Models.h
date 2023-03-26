#pragma once

#include "platform.h"
#include "Buffers.h"

struct Model
{
	u32 meshIdx;
	std::vector<u32> materialIdx;
};

struct Submesh
{
	VertexBufferLayout vertexBufferLayout;
	std::vector<float> vertices;
	std::vector<u32> indices;
	u32 vertexOffset;
	u32 indexOffset;
	std::vector<Vao> vaos;
};

struct Mesh
{
	std::vector<Submesh> submeshes;
	u32 vertexBufferHandle;
	u32 indexBufferHandle;
};

struct Material
{
	std::string name;
	glm::vec3 albedo;
	glm::vec3 emissive;
	f32 smoothness;
	u32 albedoTextureIdx;
	u32 emissiveTextureIdx;
	u32 specularTextureIdx;
	u32 normalsTextureIdx;
	u32 bumpTextureIdx;
};

struct Entity
{
	void PushEntity(u32 modelID)
	{
		worldMatrix = glm::mat4(1.0f);
		modelIndex = modelID;
	}

	glm::mat4 GetTransform() const
	{
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotation.x, { 1.0f, 0.0f, 0.0f }) *
			glm::rotate(glm::mat4(1.0f), rotation.y, { 0.0f, 1.0f, 0.0f }) *
			glm::rotate(glm::mat4(1.0f), rotation.z, { 0.0f, 0.0f, 1.0f });

		return glm::translate(glm::mat4(1.0f), position) *
			rot *
			glm::scale(glm::mat4(1.0f), scale);
	}

	glm::mat4 worldMatrix;
	u32 modelIndex;
	u32 localParamsOffset;
	u32 localParamsSize;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};