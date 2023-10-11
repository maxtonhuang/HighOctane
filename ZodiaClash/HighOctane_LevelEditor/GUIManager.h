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
*	@file		GUIManager.h
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
*	@date		10 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file manages ImGui panels.Each panels will be split into their independant
*   source files.
******************************************************************************/
#pragma once
#include "ImGuiLib.h"
//#include "graphics.h"

#ifdef LEVELEDITOR_EXPORTS
#define LEVELEDITOR_API __declspec(dllexport)
#else 
#define LEVELEDITOR_API __declspec(dllimport)
#endif


LEVELEDITOR_API class GUIManager {
public:
        GUIManager();

        ~GUIManager();
/*!***********************************************************************
    \brief
    Init manager to call ImGui Panels
    \param [in] window
    GLFWwindow for ImGui to work on
*************************************************************************/
        void Init();
/*!***********************************************************************
 \brief
    Update manager to call ImGui Panels
 \param [in] window
    GLFWwindow for ImGui to work on
 *************************************************************************/
        void Update();

private:
  

};


    //extern GUIManager guiManager;
