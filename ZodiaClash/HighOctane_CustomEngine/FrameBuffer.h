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
*	@file		FrameBuffer.h
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

#pragma once
#include "graphlib.h"
#include "GraphicConstants.h"

class FrameBuffer {
public:
	void Initialize();
	void Delete();
	void Recreate();
	void Bind();
	void Unbind();
	void Clear();

	unsigned int GetTextureID();
private:
	GLuint id; //framebuffer id
	GLuint textureid; //resultant texture id
	GLuint rbo; //depth and stencil buffer id
};
