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
*	@file		ImGuiComponents.h
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
*	@date		11 November 2023
*
* *****************************************************************************
*
*	@brief		Component viewer window
*
*   Contains function for the component window as well as
*	saving prefab in IMGUI
*
******************************************************************************/

#pragma once
#include "ImGuiLib.h"
#include "ECS.h"

//Updates the selected scene entities component viewer
void UpdateComponentViewer();

//Shows the component browser for input entity
void ComponentBrowser(Entity);

//Saves the input entity as a prefab in prefabPath
void SaveAsPrefab(std::string prefabPath, Entity);
