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
*	@file		ImGuiSceneHiearchy.h
*
*	@author		Kai Alexander Van Adrichem Boogaert (Scene Hiaerarchy and Properties panel)
*
*	@email		kaialexander.v\@digipen.edu
*
*
*	@co-author	Foong Pun Yuen Nigel (Prefabs editor)
*
*	@email		p.foong\@digipen.edu
*
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		09 November 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains all the dedclarations for scene hierarchy, prefabs editor,
*	and properties editor panel.
******************************************************************************/
#pragma once
#include "ImGuiLib.h"
#include "ECS.h"

extern Entity currentSelectedEntity;
extern Entity currentSelectedPrefab;

void UpdateSceneHierachy();

//Update for prefab window
void UpdatePrefabHierachy();
void SceneEntityNode(Entity entity);
void SceneEntityComponents(Entity entity);
