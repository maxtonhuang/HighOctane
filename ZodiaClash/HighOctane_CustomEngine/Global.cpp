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
#include "Editing.h"

#define RESET_VEC2 0.f, 0.f
#define RESET_VEC4 0.f, 0.f, 0.f, 0.f

// Declare global variables here

//HCURSOR hCustomCursor{};
GLFWcursor * customCursor{ nullptr };

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
bool pauseBuffer{ false };
bool stopBuffer{ false };
bool loadingBuffer{ false };
bool sceneChangedWhilePlay{ false };
bool rightClick{ false };
bool toDestroy{ false };
bool toCopy{ false };
bool inEditing{ false };
bool viewportWindowHovered{ false };
bool goatSpeedup{ false };
bool fullyDeleteLayer{ false };
std::unordered_map<std::string, std::shared_ptr<ComponentFunctions>> typeMap;
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
vmath::Vector2 prevMousePosition{ RESET_VEC2 };
bool mouseMoved{ false };
bool somethingChangedCursor{ false };
bool withinSomething{ false };
Entity keyObjectID{ std::numeric_limits<Entity>().max() };
glm::vec4 keyObjectColor{ RESET_VEC4 };
bool snappingOn{ true };
bool tilemapOn{ false };
float gridSpacingX{ 200.f };
float gridSpacingY{ 200.f };
float gridOffsetX{ 0.f };
float gridOffsetY{ 0.f };
int gridAlignment{ SA_CENTER };

Entity settingsEntityBeingDragged{ std::numeric_limits<Entity>().max() };


std::unordered_map<Entity, glm::vec4> snappingHighlight{};
std::unordered_map<Entity, std::unordered_map<bool, std::tuple<vmath::Vector2, vmath::Vector2, vmath::Vector2, vmath::Vector2>>> snappingLines{};
std::set<Entity> centerVertical{};
std::set<Entity> intersectVertical{};
std::set<Entity> centerHorizontal{};
std::set<Entity> intersectHorizontal{};
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

bool fullscreen{ false };

CursorEditingTooltip cursorEditingTooltipState{ CursorEditingTooltip::NONE };

/******************************************************************************
*
*	@brief Converts the System Mode enum to a string literal
*
*	-
*
******************************************************************************/
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

/******************************************************************************
*
*	@brief Sets the System Mode
*
*	-
*
******************************************************************************/
void SetCurrentSystemMode(SystemMode mode)
{
	if (mode == GetCurrentSystemMode()) {
		return;
	}
	systemModeStack.pop_front();
	systemModeStack.emplace_back(mode);
}

/******************************************************************************
*
*	@brief Returns the current System Mode
*
*	
*
******************************************************************************/
SystemMode GetCurrentSystemMode() {

	if (systemModeStack.empty()) {
		return SystemMode::NONE;
	}

	return systemModeStack.back();
}

/******************************************************************************
*
*	@brief returns the previous System Mode from the stack
*
*	
*
******************************************************************************/
SystemMode GetPreviousSystemMode() {
	
	if (systemModeStack.size() < 2) {
		return SystemMode::NONE;
	}

	return systemModeStack[systemModeStack.size() - 2];

}

/******************************************************************************
*
*	@brief Returns the System Mode from two modes back
*
*	-
*
******************************************************************************/
SystemMode GetPreviousPreviousSystemMode() {

	if (systemModeStack.size() < 3) {
		return SystemMode::NONE;
	}
	return systemModeStack.front();
}