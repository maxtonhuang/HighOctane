#pragma once
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
*	@file		ImGuiContentBrowser.h
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		10 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file is for the ImGui Content Browser where users can simply drag and drop
*	assets from the content browser to edit assets accordingly.
*	
*	!------M2 Milestone------!
*	- Currently only allows scene files to be dragged into the scene and it will change scene
******************************************************************************/
#include "ImGuiLib.h"

void UpdateContentBrowser();
void UnloadIcons();
void LoadIcons();
