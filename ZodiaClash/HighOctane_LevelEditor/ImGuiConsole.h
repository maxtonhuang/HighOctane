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
*	@file		ImGuiConsole.h
*
*	@author		Koh Wen Yuan
*
*	@email		k.wenyuan\@digipen.edu
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
*	@brief		Console window for ImGui
*
*	This file contains all the declaration of the functions for the console window
*
******************************************************************************/
#pragma once
#include "ImGuiLib.h"
#include <iostream>
#include <fstream>
#include <string>

void InitConsole();
void UpdateConsole();

/*******************************************************************/


void ExportConsoleToFile(const char* fileName);

void DeleteLineFromFile(const char* fileName);