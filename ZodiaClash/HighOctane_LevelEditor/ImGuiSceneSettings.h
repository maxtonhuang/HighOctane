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
*	@file		ImGuiSceneSettings.h
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
*	@brief		Scene settings window
*
*   Contains function for the gscene settings window which allows
*	for showing the scene name and saving the current scene as well as
*	changing the BGM and showing loaded assets
*
******************************************************************************/

#pragma once
#include "ImGuiLib.h"

//Update function for scene settings window, to be called once every frame
void UpdateSceneSettingsWindow();
