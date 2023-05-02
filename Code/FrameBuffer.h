#pragma once

#include "platform.h"

class FrameBuffer
{
public:
	FrameBuffer(glm::vec2 displaySize);
	~FrameBuffer();

	void Bind();
	void Unbind();

	void DrawAttachments(u32 count, u32 attachments[]);

public:
	u32 colorAttachmentAlbedoId;
	u32 colorAttachmentNormalsId;
	u32 colorAttachmentPositionId;
	u32 depthAttachmentId;
	u32 colorAttachmentSpecularId;
	u32 rendererID;
private:
	void SetupFrameBuffer(glm::vec2 displaySize);

private:
	

	
};