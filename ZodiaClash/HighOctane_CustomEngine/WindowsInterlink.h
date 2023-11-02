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
*	@file		WindowsInterlink.h
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
*	@date		3 November 2023
*
* *****************************************************************************
*
*	@brief		Function declarations to functions in WindowsInterlink.cpp
*
*	-
*
******************************************************************************/

#pragma once

#include <vector>
#include <string>
#include "graphlib.h"

void OpenFileDialog();

void FileDropCallback(GLFWwindow* window, int count, const char** paths);

std::string SaveFileDialog(std::string extensionName, std::string extensionDescription);

std::string OpenSingleFileDialog();