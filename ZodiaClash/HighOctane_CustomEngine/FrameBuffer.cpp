#include "FrameBuffer.h"
#include "debugdiagnostic.h"

void FrameBuffer::Initialize() {
	//create framebuffer
	glCreateFramebuffers(1, &id);
	Bind();

	//create framebuffer texture
	glCreateTextures(GL_TEXTURE_2D, 1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GRAPHICS::defaultWidth, GRAPHICS::defaultHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureid, 0);

	GLuint rbo;
	glCreateTextures(GL_TEXTURE_2D, 1, &rbo);
	glBindTexture(GL_TEXTURE_2D, rbo);
	//glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, GRAPHICS::defaultWidth, GRAPHICS::defaultHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rbo, 0);

	ASSERT(!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Unable to create framebuffer!");

	Unbind();
}

void FrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int FrameBuffer::GetTextureID() {
	return textureid;
}
