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
#include "graphics.h"

/*!***********************************************************************
 \brief
  Initializes the framebuffer by creating a new framebuffer object, texture for color attachment, and a texture for depth and stencil attachment. It sets up the necessary parameters for these objects, including their filtering modes and attachments to the framebuffer. This method should be called to set up the framebuffer before it is used for rendering.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It initializes the framebuffer with a color attachment and a depth-stencil attachment based on the current window dimensions. It also ensures that the framebuffer is complete and ready for use.
 *************************************************************************/
void FrameBuffer::Initialize() {
	//create framebuffer
	glCreateFramebuffers(1, &id);
	Bind();

	//create framebuffer texture
	glCreateTextures(GL_TEXTURE_2D, 1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)graphics.GetWindowWidth(), (GLsizei)graphics.GetWindowHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureid, 0);

	//create depth and stencil buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &rbo);
	glBindTexture(GL_TEXTURE_2D, rbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, (GLsizei)graphics.GetWindowWidth(), (GLsizei)graphics.GetWindowHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rbo, 0);

	ASSERT(!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Unable to create framebuffer!");

	glClearColor(0.f, 0.f, 0.f, 1.f);

	Unbind();
}

/*!***********************************************************************
 \brief
  Deletes the framebuffer and its associated texture and renderbuffer objects from memory. This method should be called when the framebuffer is no longer needed or before the framebuffer object is recreated.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It directly frees the OpenGL resources associated with the framebuffer, its texture, and renderbuffer.
 *************************************************************************/
void FrameBuffer::Delete() {
	glDeleteFramebuffers(1, &id);
	glDeleteTextures(1, &textureid);
	glDeleteTextures(1, &rbo);
}

/*!***********************************************************************
 \brief
  Recreates the framebuffer by first deleting the current OpenGL framebuffer, texture, and renderbuffer objects, and then reinitializing them. This method is useful for when the properties of the framebuffer need to be reset or changed.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It ensures the framebuffer is refreshed with new OpenGL resources by deleting and reinitializing them.
 *************************************************************************/
void FrameBuffer::Recreate() {
	Delete();
	Initialize();
}

/*!***********************************************************************
 \brief
  Binds the framebuffer object for rendering operations. After this method is called, any subsequent rendering commands will affect this framebuffer until it is unbound.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It changes the current rendering target to this framebuffer object.
 *************************************************************************/
void FrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

/*!***********************************************************************
 \brief
  Unbinds the current framebuffer, reverting the rendering target back to the default framebuffer provided by OpenGL. This method should be called after finishing rendering to a custom framebuffer.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It resets the rendering target to the default framebuffer.
 *************************************************************************/
void FrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*!***********************************************************************
 \brief
  Clears the framebuffer by binding it and then clearing its color buffer. This is typically used at the beginning of a new frame to reset the framebuffer's contents.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It clears the contents of the framebuffer to prepare it for new rendering operations.
 *************************************************************************/
void FrameBuffer::Clear() {
	Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	Unbind();
}

/*!***********************************************************************
 \brief
  Retrieves the texture ID associated with the framebuffer. This texture ID can be used to access the framebuffer's color attachment for use in shaders or other OpenGL operations.
 \param
  This method does not take any parameters.
 \return
  Returns the unsigned integer ID of the texture associated with the framebuffer.
 *************************************************************************/
unsigned int FrameBuffer::GetTextureID() {
	return textureid;
}
