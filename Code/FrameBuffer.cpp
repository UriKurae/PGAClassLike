#include "FrameBuffer.h"

#include <glad/glad.h>

FrameBuffer::FrameBuffer(glm::vec2 displaySize, std::vector<int> attachments)
{
	SetupFrameBuffer(displaySize, attachments);
}

FrameBuffer::~FrameBuffer()
{
}

/*void FrameBuffer::SetupFrameBuffer(glm::vec2 displaySize)
{
	glGenTextures(1, &colorAttachmentAlbedoId);
	glBindTexture(GL_TEXTURE_2D, colorAttachmentAlbedoId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, displaySize.x, displaySize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &colorAttachmentNormalsId);
	glBindTexture(GL_TEXTURE_2D, colorAttachmentNormalsId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, displaySize.x, displaySize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &colorAttachmentPositionId);
	glBindTexture(GL_TEXTURE_2D, colorAttachmentPositionId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, displaySize.x, displaySize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &colorAttachmentSpecularId);
	glBindTexture(GL_TEXTURE_2D, colorAttachmentSpecularId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, displaySize.x, displaySize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenTextures(1, &depthAttachmentId);
	glBindTexture(GL_TEXTURE_2D, depthAttachmentId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, displaySize.x, displaySize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &rendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorAttachmentAlbedoId, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, colorAttachmentNormalsId, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, colorAttachmentPositionId, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, colorAttachmentSpecularId, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachmentId, 0);

	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (framebufferStatus)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			ELOG("framebuffer not defined!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			ELOG("Incomplete attachment!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			ELOG("missing attachment!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			ELOG("Incomplete draw buffer!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			ELOG("Incomplete read buffer!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			ELOG("Framebuffer not supported!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			ELOG("Incomplete multisampling!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			ELOG("Incomplete layer targets!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		}
	}

	GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}*/

void FrameBuffer::SetupFrameBuffer(glm::vec2 displaySize, std::vector<int> attachments)
{
	fbSpecifications = attachments;
	for (int i = 0; i < attachments.size(); ++i)
	{
		u32 attachmentId = 0;
		glGenTextures(1, &attachmentId);
		glBindTexture(GL_TEXTURE_2D, attachmentId);
		glTexImage2D(GL_TEXTURE_2D, 0, attachments[i], displaySize.x, displaySize.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		colorAttachments.push_back(attachmentId);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glGenTextures(1, &depthAttachmentId);
	glBindTexture(GL_TEXTURE_2D, depthAttachmentId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, displaySize.x, displaySize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachmentId, 0);

	glGenFramebuffers(1, &rendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
	for (int i = 0; i < attachments.size(); i++)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, colorAttachments[i], 0);
	}

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachmentId, 0);

	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (framebufferStatus)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			ELOG("framebuffer not defined!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			ELOG("Incomplete attachment!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			ELOG("missing attachment!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			ELOG("Incomplete draw buffer!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			ELOG("Incomplete read buffer!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			ELOG("Framebuffer not supported!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			ELOG("Incomplete multisampling!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			ELOG("Incomplete layer targets!");
			assert(framebufferStatus == GL_FRAMEBUFFER_COMPLETE);
			break;
		}
	}

	std::vector<GLuint> attachmentsDraw = {};
	for (int i = 0; i < attachments.size(); ++i)
	{
		attachmentsDraw.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	
	glDrawBuffers(attachmentsDraw.size(), attachmentsDraw.data());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void FrameBuffer::DeleteFrameBuffers()
{
	glDeleteFramebuffers(1, &rendererID);
	glDeleteTextures(colorAttachments.size(), colorAttachments.data());
	glDeleteTextures(1, &depthAttachmentId);
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::DrawAttachments(u32 count, u32 attachments[])
{
	glDrawBuffers(count, attachments);
}

void FrameBuffer::Resize(glm::vec2 newDisplaySize)
{
	if (newDisplaySize.x <= 0.0f || newDisplaySize.y <= 0.0f)
		return;

	DeleteFrameBuffers();
	SetupFrameBuffer(newDisplaySize, fbSpecifications);
}
