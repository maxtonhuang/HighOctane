/*
\copyright
        All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
        reserved.Reproduction or disclosure of this file or its contents without the prior
        written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		ImGuiConsole.h
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		23 September 2023
@brief		This file contains the functions declaration for Console window in ImGui

*//*______________________________________________________________________*/
#pragma once
#include "ImGuiLib.h"
#include <iostream>
#include <fstream>
#include <string>

void InitConsole(GLFWwindow* window);
void UpdateConsole(GLFWwindow* window);

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