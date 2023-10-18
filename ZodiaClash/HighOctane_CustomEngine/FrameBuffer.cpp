/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		FrameBuffer.cpp
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		18 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains functions for game frame buffer
*	Entire game is rendered as a frame buffer texture and then
*	drawn to the screen
*
******************************************************************************/

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

	//create depth and stencil buffer
	GLuint rbo;
	glCreateTextures(GL_TEXTURE_2D, 1, &rbo);
	glBindTexture(GL_TEXTURE_2D, rbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, GRAPHICS::defaultWidth, GRAPHICS::defaultHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rbo, 0);

	ASSERT(!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Unable to create framebuffer!");

	glClearColor(0.f, 0.f, 0.f, 1.f);

	Unbind();
}

void FrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Clear() {
	Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	Unbind();
}

unsigned int FrameBuffer::GetTextureID() {
	return textureid;
}
