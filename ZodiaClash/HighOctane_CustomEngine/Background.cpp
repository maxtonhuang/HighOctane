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

/**
 * @brief Cconstructor
 *
 * Constructor of the background system
 */
BackgroundSystem::BackgroundSystem() {
	scrolldata = new float[GRAPHICS::vertexBufferSize];
	count = 0;
}

/**
 * @brief Destructor
 *
 * Destructor of the background system
 */
BackgroundSystem::~BackgroundSystem() {
	delete[] scrolldata;
}

/**
 * @brief Update
 *
 * Main update function of the background system
 */
void BackgroundSystem::Update() {
	if (!ECS::ecs().HasComponent<Transform>(focus)) {
		count = 0;
		return;
	}
	float scrolltarget = ECS::ecs().GetComponent<Transform>(focus).position.x / GRAPHICS::w;
	graphics.renderer["parallax"].UpdateUniform1fv("scrolltarget", &scrolltarget);
	graphics.renderer["parallax"].UpdateUniform1fv("scrollspeed", scrolldata, count);
	count = 0;
}

/**
 * @brief SetFocusEntity
 *
 * Sets the focused entity for the background to center around
 */
void BackgroundSystem::SetFocusEntity(Entity entity) {
	focus = entity;
}

/**
 * @brief AddBackground
 *
 * Add a background to the background system
 */
void BackgroundSystem::AddBackground(float scroll) {
	ASSERT(count > GRAPHICS::vertexBufferSize, "Background system error!");
	scrolldata[count] = scroll;
	count++;
}
