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
*	@file		Camera.h
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
*	@date		8 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains class and functions for in-game camera
*
******************************************************************************/
#pragma once
#include "graphlib.h"
#include "VMath.h"

class Camera {
public:
	Camera(); //constructor to initialise values

	void Update(); //pass camera values to shaders

	void Reset(); //reset camera position and scale

	void SetPos(float x, float y); //set camera position
	void AddPos(float x, float y); //add camera position
	vmath::Vector2 GetPos();

	void SetZoom(float zoom); //scale for camera (eg 1.0 for default, 2.0 for 2x)
	void AddZoom(float zoom); //scale for camera (eg 1.0 for default, 2.0 for 2x)
private:
	vmath::Vector2 pos{};
	float scale{};
};

extern Camera camera;
