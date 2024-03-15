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
*	@file		Tutorial.h
*
*	@co-author	Chua Zhen Rong
*
*	@email		c.zhenrong\@digipen.edu
*
*   @author		Foong Pun Yuen Nigel (core system structure outline)
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		[M5] 15 March 2024
*
* *****************************************************************************
*
*	@brief		Event handling and automation of tutiorial system
*
*	Contains logic and event handling for the tutorial system. Layer management
*	and storing of tutorials state is handled by the system, and makes use of
*	on click event triggers to advance the tutorial.
*
******************************************************************************/

#pragma once
#include "ECS.h"
#include "Global.h"
#include "Battle.h"

class UITutorialSystem : public System {
public:
	Entity overlay{ 0 };
	Entity currentTutorialEntity{ 0 };
	int stepIndex{ 0 };
	bool overlayOn{ false };
	bool systemOverlayOn{ false };
	std::vector<std::pair<Entity, size_t>> originalLayers;
	std::vector<std::pair<Entity, size_t>> modifiedLayers;

	void Initialize();
	void Update() override;
	void UpdateState();
	void CheckConditionFulfilled(bool& result);
	void SurfaceSystemOverlay(Entity& entity);
	void MaintainLayers();
private:
	vmath::Vector2 tutMousePos{ RESET_VEC2 };
	bool prefabOffset { false };
	bool nextStepWait { false };
	bool tutorialComplete{ false };

	void GetChildren(std::vector<Entity>& entityList);
	void SurfaceTargetLayers(const std::vector<Entity> entities);
	void RevertLayers();
	
};