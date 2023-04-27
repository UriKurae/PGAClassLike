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
	u32 colorAttachmentId;
	u32 colorAttachmentNormalsId;
	u32 colorAttachmentDepthId;
	u32 colorAttachmentPositionId;
	u32 depthAttachmentId;
	u32 rendererID;
private:
	void SetupFrameBuffer(glm::vec2 displaySize);

private:
	

	
};