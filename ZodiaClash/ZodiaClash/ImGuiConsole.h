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
// ALL THESE IS FOR DEBUGGING PLEASE DO NOT TOUCH AT ALL
// This is the stream buffer that will be used to redirect std::cout to ImGui
class ImGuiOutputBuffer : public std::streambuf {
public:
    int overflow(int c);
    const std::string& GetBuffer() const;
    void ClearBuffer();
    std::string buffer;
private:

};

extern ImGuiOutputBuffer imguiOutputBuffer; // Declare as extern for use in other files

void ExportConsoleToFile(const char* fileName);

void DeleteLineFromFile(const char* fileName);