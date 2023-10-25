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

//#include <mutex>
#include <memory>
#include "ECS.h"
#include <deque>
#include <vector>
#include <string>

// Declare global variables here

float g_dt;
bool button_clicked{ false };
bool edit_mode{ true };
bool hoveringPanel{ false };
bool fileDropped{ false };
float dropTimer{};
bool destroyAll{ false };
bool saveFile{ false };

std::shared_ptr<SerializationSystem> s_ptr;
std::deque<std::string> importFileList;
size_t importFileCount{ };
std::vector<std::string> fullNameVecImGUI{};