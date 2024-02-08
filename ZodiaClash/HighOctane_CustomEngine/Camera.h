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

using Entity = std::uint32_t;

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
	float GetZoom(); //Gets the zoom value of the camera

	void SetTarget(Entity entity); //Sets the target for the camera, the camera will then follow this entity
	void DetachTarget(); //Detach the camera from its currently following entity

	void SetShake(float shake); //Set the shake value for camera shake
	void AddShake(float shake); //Adds shake to the camera

private:
	vmath::Vector2 pos{}; //camera position
	float scale{}; //camera zoom
	Entity target{}; //which entity the camera is following
	float magnitude{}; //camera magnitude
};

extern Camera camera;
