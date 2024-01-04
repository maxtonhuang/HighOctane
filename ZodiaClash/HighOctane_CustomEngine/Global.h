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

////////// Defines for fixed dt system. ///////////////////////////////////////
#define FIXED_DT 1.0f/60.f
#define MAX_ACCUMULATED_TIME 5.f //to avoid the "spiral of death" if the system cannot keep up
///////////////////////////////////////////////////////////////////////////////

const HCURSOR hDefaultCursor{ LoadCursor(NULL, IDC_ARROW) };
const HCURSOR hNWSECursor{ LoadCursor(NULL, IDC_SIZENWSE) };
const HCURSOR hNESWCursor{ LoadCursor(NULL, IDC_SIZENESW) };
const HCURSOR hAllDirCursor{ LoadCursor(NULL, IDC_SIZEALL) };

extern float g_dt;
extern float accumulatedTime;
extern bool button_clicked;
extern bool hoveringPanel;
extern bool fileDropped;
extern float dropTimer;
extern bool destroyAll;
extern bool saveFile;
extern bool rightClick;
extern bool playButton;
extern bool stopButton;
extern bool sceneChangedWhilePlay;
extern bool toDestroy;
extern bool toCopy;
extern bool inEditing;
extern bool viewportWindowHovered;
extern vmath::Vector2 rightClickPos;
extern std::vector<Entity> selectedEntities; // for destruction
extern Entity newSelection;
extern bool popupHovered;
extern size_t selectedCount;
extern bool popupMasterRightClicked;
extern vmath::Vector2 draggingOffset;
extern bool thereWasAClickThisCycle;
extern bool somethingWasSelectedThisCycle;
extern bool draggingThisCycle;
extern vmath::Vector2 currentMousePosition;
extern bool mouseMoved;
extern bool withinSomething;
extern Entity keyObjectID;

extern bool shiftKeyPressed;
extern bool controlKeyPressed;

extern std::unordered_set<std::string> defaultUniqueComponents;

extern bool clearAllSelection;
extern std::string sceneName;
extern bool newScene;
extern bool initLevel;
extern std::string newSceneName;

extern size_t selectedLayer;
extern size_t layerCounter; 
extern size_t selectedGroup;
extern size_t groupCounter; 
extern std::deque<std::pair<std::string, bool>> layerNames;
extern std::deque< std::deque<Entity> > layering;
extern std::array<bool, 10000> layersToSkip;
extern std::array<bool, 100000> entitiesToSkip;
extern std::array<bool, 10000> layersToLock;
extern std::array<bool, 100000> entitiesToLock;

extern std::shared_ptr<EditingSystem> edit_ptr;
extern std::shared_ptr<SerializationSystem> s_ptr;
extern std::deque<std::string> importFileList;
extern size_t importFileCount;
extern std::vector<std::string> fullNameVecImGUI;

extern std::string currentScriptForIMGUI;

extern std::unordered_map<Entity, std::vector<std::string>> scriptNamesAttachedforIMGUI;

extern std::string currentScriptAttachedForIMGUI;

extern bool isHotReload;

enum class SystemMode 
{
	RUN = 0,
	EDIT,
	PAUSE,
	GAMEHELP,
	EXITCONFIRM,
	NONE
};

std::string SystemModeToString(SystemMode mode);

extern std::deque<SystemMode> systemModeStack;

extern void SetCurrentSystemMode(SystemMode mode);
extern SystemMode GetCurrentSystemMode();
extern SystemMode GetPreviousSystemMode();
extern SystemMode GetPreviousPreviousSystemMode();
