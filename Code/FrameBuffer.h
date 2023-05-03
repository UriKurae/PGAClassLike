#pragma once

#include "platform.h"

class FrameBuffer
{
public:
	FrameBuffer(glm::vec2 displaySize, std::vector<int> attachments);
	~FrameBuffer();

	void Bind();
	void Unbind();

	void DrawAttachments(u32 count, u32 attachments[]);

public:
	u32 colorAttachmentAlbedoId;
	u32 colorAttachmentNormalsId;
	u32 colorAttachmentPositionId;
	u32 colorAttachmentSpecularId;

	u32 rendererID;
	std::vector<u32> colorAttachments;
	u32 depthAttachmentId;
private:
	//void SetupFrameBuffer(glm::vec2 displaySize);
	void SetupFrameBuffer(glm::vec2 displaySize, std::vector<int> attachments);

private:
	

	
};