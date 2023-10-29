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
*	@file		Global.h
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		Extern for Global variables
*
*	This file holds contains the extern declarations for global variables.
*	Programmers only need to #include this header file to be able to 
*	automatically link and access the variables in their cpp source files.
*
******************************************************************************/

#pragma once

#include "ECS.h"
#include "vmath.h"
#include <deque>
#include <vector>
#include <string>
#include <memory>

#define RESET_VEC2 0.f, 0.f

const HCURSOR hDefaultCursor{ LoadCursor(NULL, IDC_ARROW) };
const HCURSOR hNWSECursor{ LoadCursor(NULL, IDC_SIZENWSE) };
const HCURSOR hNESWCursor{ LoadCursor(NULL, IDC_SIZENESW) };
const HCURSOR hAllDirCursor{ LoadCursor(NULL, IDC_SIZEALL) };

extern float g_dt;
extern bool button_clicked;
extern bool edit_mode;
extern bool hoveringPanel;
extern bool fileDropped;
extern float dropTimer;
extern bool destroyAll;
extern bool saveFile;
extern bool rightClick;
extern bool playButton;
extern bool stopButton;
extern bool toDestroy;
extern vmath::Vector2 rightClickPos;
extern std::vector<Entity> selectedEntities;
extern Entity newSelection;
extern bool popupHovered;
extern bool anyObjectSelected;
extern bool newScene;
extern std::string newSceneName;

extern std::shared_ptr<SerializationSystem> s_ptr;
extern std::deque<std::string> importFileList;
extern size_t importFileCount;
extern std::vector<std::string> fullNameVecImGUI;


extern const char* currentScriptForIMGUI;