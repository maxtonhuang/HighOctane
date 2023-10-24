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
*	@file		Camera.cpp
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
#include "Camera.h"
#include "graphics.h"

Camera camera;

Camera::Camera() {
	scale = 1.f;
}

void Camera::Update() {
	glm::mat3 matrix{ scale,0,0,0,scale,0,pos.x / GRAPHICS::w, pos.y / GRAPHICS::h, 1 };
	for (auto& r : graphics.renderer) {
		r.second.UpdateUniformMatrix3fv("uCamera", &matrix);
	}
}

void Camera::Reset() {
	pos = vmath::Vector2{ 0,0 };
	scale = 1.f;
	Update();
}

void Camera::SetPos(float x, float y) {
	pos.x = x;
	pos.y = y;
	Update();
}

void Camera::AddPos(float x, float y) {
	pos.x += x;
	pos.y += y;
	Update();
}

vmath::Vector2 Camera::GetPos() {
	return pos;
}

void Camera::SetZoom(float zoom) {
	scale = zoom;
	Update();
}

void Camera::AddZoom(float zoom) {
	scale += zoom;
	if (scale < 0.f) {
		scale = 0.f;
	}
	Update();
}
