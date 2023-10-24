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
*	@file		Viewport.cpp
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
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains class and functions for game viewport
*
******************************************************************************/
#include "Viewport.h"
#include "graphics.h"

void Viewport::Use() {
	glViewport(x, y, w, h);
}

void Viewport::Unuse() {
	glViewport(0, 0, (GLsizei)graphics.GetWindowWidth(), (GLsizei)graphics.GetWindowHeight());
}

void Viewport::SetViewport(int input_x, int input_y, unsigned int input_w, unsigned int input_h) {
	x = input_x;
	y = input_y;
	w = input_w;
	h = input_h;
	glViewport(x, y, w, h);
}

void Viewport::Resize(float input) {
	w = (unsigned int)((float)w * input);
	h = (unsigned int)((float)h * input);
	glViewport(x, y, w, h);
}

int Viewport::GetX() {
	return x;
}

int Viewport::GetY() {
	return y;
}

unsigned int Viewport::GetW() {
	return w;
}

unsigned int Viewport::GetH() {
	return h;
}
