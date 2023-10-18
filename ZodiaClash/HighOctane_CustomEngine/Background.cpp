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
*	@file		Background.cpp
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
*	@date		19 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains class for background system used for parallax
*	background rendering
*
******************************************************************************/

#include "Background.h"
#include "graphics.h"

BackgroundSystem::BackgroundSystem() {
	scrolldata = new float[GRAPHICS::vertexBufferSize];
	count = 0;
}

BackgroundSystem::~BackgroundSystem() {
	delete[] scrolldata;
}

void BackgroundSystem::Update() {
	if (focus == 0) {
		return;
	}
	float scrolltarget = ECS::ecs().GetComponent<Transform>(focus).position.x / GRAPHICS::w;
	parallaxRenderer.UpdateUniform1fv("scrolltarget", &scrolltarget);
	parallaxRenderer.UpdateUniform1fv("scrollspeed", scrolldata, count);
	count = 0;
}

void BackgroundSystem::SetFocusEntity(Entity entity) {
	focus = entity;
}

void BackgroundSystem::AddBackground(float scroll) {
	scrolldata[count] = scroll;
	count++;
}
