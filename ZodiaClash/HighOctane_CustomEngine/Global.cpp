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
*	@file		Global.cpp
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
*	@brief		Global variables
*
*	This file holds global variables that need to be used in multiple source
*	files.
*
******************************************************************************/

#include "ECS.h"
#include "vmath.h"
#include <deque>
#include <vector>
#include <string>
#include <memory>
#include <limits>
#include "Components.h"
#include "model.h"
#include "Global.h"

#define RESET_VEC2 0.f, 0.f


// Declare global variables here

float g_dt;
float accumulatedTime{};
bool button_clicked{ true };
bool hoveringPanel{ false };
bool fileDropped{ false };
float dropTimer{};
bool destroyAll{ false };
bool saveFile{ false };
bool playButton{ false };
bool stopButton{ false };
bool sceneChangedWhilePlay{ false };
bool rightClick{ false };
bool toDestroy{ false };
bool toCopy{ false };
bool inEditing{ false };
bool viewportWindowHovered{ false };
vmath::Vector2 rightClickPos{ RESET_VEC2 };

std::vector<Entity> selectedEntities{};
Entity newSelection{};
bool popupHovered{ false };
size_t selectedCount{};
bool popupMasterRightClicked{ false };
vmath::Vector2 draggingOffset{ RESET_VEC2 };
bool thereWasAClickThisCycle{ false };
bool somethingWasSelectedThisCycle{ false };
bool draggingThisCycle{ false };
vmath::Vector2 currentMousePosition{ RESET_VEC2 };
bool mouseMoved{ false };
bool withinSomething{ false };
Entity keyObjectID{ std::numeric_limits<Entity>().max() };

bool shiftKeyPressed{ false };
bool controlKeyPressed{ false };

std::unordered_set<std::string> defaultUniqueComponents{ typeid(Transform).name(), typeid(Name).name(), typeid(Model).name() };

bool clearAllSelection{ false };
std::string sceneName{};
bool newScene{ true };
bool initLevel{ true };
std::string newSceneName{};

size_t selectedLayer{ std::numeric_limits<size_t>::max() };

size_t layerCounter{};
size_t selectedGroup{};
size_t groupCounter{};
std::deque<std::pair<std::string, bool>> layerNames{};
std::deque< std::deque<Entity> > layering{};
std::array<bool, 10000> layersToSkip{};
std::array<bool, 100000> entitiesToSkip{};
std::array<bool, 10000> layersToLock{};
std::array<bool, 100000> entitiesToLock{};

std::shared_ptr<EditingSystem> edit_ptr;
std::shared_ptr<SerializationSystem> s_ptr;
std::deque<std::string> importFileList;
size_t importFileCount{ };
std::vector<std::string> fullNameVecImGUI{};

std::string currentScriptForIMGUI{};

std::unordered_map<Entity, std::vector<std::string>> scriptNamesAttachedforIMGUI{};

std::string currentScriptAttachedForIMGUI{};

bool isHotReload{ false };


std::string SystemModeToString(SystemMode mode)
{
	switch (mode)
	{
	case SystemMode::RUN:
		return "RUN";
	case SystemMode::EDIT:
		return "EDIT";
	case SystemMode::PAUSE:
		return "PAUSE";
	case SystemMode::GAMEHELP:
		return "GAMEHELP";
	case SystemMode::EXITCONFIRM:
		return "EXITCONFIRM";
	case SystemMode::NONE:
		return "NONE";
	default:
		return "NONE";
	}
}

std::deque<SystemMode> systemModeStack = { SystemMode::NONE, SystemMode::NONE, SystemMode::EDIT };

void SetCurrentSystemMode(SystemMode mode)
{
	if (mode == GetCurrentSystemMode()) {
		return;
	}
	systemModeStack.pop_front();
	systemModeStack.emplace_back(mode);
}

SystemMode GetCurrentSystemMode() {

	if (systemModeStack.empty()) {
		return SystemMode::NONE;
	}

	return systemModeStack.back();
}

SystemMode GetPreviousSystemMode() {
	
	if (systemModeStack.size() < 2) {
		return SystemMode::NONE;
	}

	return systemModeStack[systemModeStack.size() - 2];

}

SystemMode GetPreviousPreviousSystemMode() {

	if (systemModeStack.size() < 3) {
		return SystemMode::NONE;
	}
	return systemModeStack.front();
}