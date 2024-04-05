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

/*!***********************************************************************
 \brief
  Sets the OpenGL viewport to this viewport's dimensions. This method is used to apply the viewport's size and position for rendering.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It sets the OpenGL viewport parameters to those of this viewport instance.
 *************************************************************************/
void Viewport::Use() {
	glViewport(x, y, w, h);
}

/*!***********************************************************************
 \brief
  Resets the OpenGL viewport to the full dimensions of the current window. This method is typically called to revert the viewport to its default state covering the entire window.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It resets the OpenGL viewport to cover the entire window.
 *************************************************************************/
void Viewport::Unuse() {
	glViewport(0, 0, (GLsizei)graphics.GetWindowWidth(), (GLsizei)graphics.GetWindowHeight());
}

/*!***********************************************************************
 \brief
  Sets the viewport's position and size and applies these settings to the OpenGL viewport. This method is used to define the area of the window where rendering will occur.
 \param input_x
  The x-coordinate of the viewport's lower-left corner.
 \param input_y
  The y-coordinate of the viewport's lower-left corner.
 \param input_w
  The width of the viewport.
 \param input_h
  The height of the viewport.
 \return
  This method does not return a value. It updates the viewport's dimensions and applies them to OpenGL.
 *************************************************************************/
void Viewport::SetViewport(int input_x, int input_y, unsigned int input_w, unsigned int input_h) {
	x = input_x;
	y = input_y;
	w = input_w;
	h = input_h;
	glViewport(x, y, w, h);
}

/*!***********************************************************************
 \brief
  Resizes the viewport's dimensions by a specified multiplier and updates the OpenGL viewport settings accordingly. This method is used to scale the viewport's size relative to its current dimensions.
 \param input
  The multiplier to apply to the viewport's width and height.
 \return
  This method does not return a value. It scales the viewport's dimensions and applies the updated size to OpenGL.
 *************************************************************************/
void Viewport::Resize(float input) {
	w = (unsigned int)((float)w * input);
	h = (unsigned int)((float)h * input);
	glViewport(x, y, w, h);
}

/*!***********************************************************************
 \brief
  Retrieves the x-coordinate of the viewport. These methods provide access to the individual components of the viewport's dimensions and position.
 \param
  These methods do not take any parameters.
 \return
  Returns the x-coordinate of the viewport as an [int/unsigned int].
 *************************************************************************/
int Viewport::GetX() {
	return x;
}

/*!***********************************************************************
 \brief
  Retrieves the y-coordinate of the viewport. These methods provide access to the individual components of the viewport's dimensions and position.
 \param
  These methods do not take any parameters.
 \return
  Returns the y-coordinate of the viewport as an [int/unsigned int].
 *************************************************************************/
int Viewport::GetY() {
	return y;
}

/*!***********************************************************************
 \brief
  Retrieves the width of the viewport. These methods provide access to the individual components of the viewport's dimensions and position.
 \param
  These methods do not take any parameters.
 \return
  Returns the width of the viewport as an [int/unsigned int].
 *************************************************************************/
unsigned int Viewport::GetW() {
	return w;
}

/*!***********************************************************************
 \brief
  Retrieves the height of the viewport. These methods provide access to the individual components of the viewport's dimensions and position.
 \param
  These methods do not take any parameters.
 \return
  Returns the height of the viewport as an [int/unsigned int].
 *************************************************************************/
unsigned int Viewport::GetH() {
	return h;
}
