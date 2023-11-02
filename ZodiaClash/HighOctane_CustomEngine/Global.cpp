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

#define RESET_VEC2 0.f, 0.f

// Declare global variables here

float g_dt;
float accumulatedTime{};
bool button_clicked{ true };
bool edit_mode{ true };
bool hoveringPanel{ false };
bool fileDropped{ false };
float dropTimer{};
bool destroyAll{ false };
bool saveFile{ false };
bool playButton{ true };
bool stopButton{ false };
bool rightClick{ false };
bool toDestroy{ false };
bool toCopy{ false };
bool inEditing{ false };
bool viewportWindowHovered{ false };
vmath::Vector2 rightClickPos{ RESET_VEC2 };

std::vector<Entity> selectedEntities{};
Entity newSelection{};
bool popupHovered{ false };
bool anyObjectSelected{ false };
bool popupMasterRightClicked{ false };

bool clearAllSelection{ false };
std::string sceneName{};
bool newScene{ true };
bool initLevel{ true };
std::string newSceneName{};

size_t currentLayer{ std::numeric_limits<size_t>::max() };
size_t selectedLayer{ std::numeric_limits<size_t>::max()  };

size_t layerCounter{};
size_t selectedGroup{};
size_t groupCounter{};
std::deque<std::string> layerNames{};
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

const char* currentScriptForIMGUI{ NULL };


std::unordered_map<Entity, std::vector<const char*>> scriptNamesAttachedforIMGUI{};

const char* currentScriptAttachedForIMGUI{ NULL };