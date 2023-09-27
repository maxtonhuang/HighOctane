/*
\copyright
        All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
        reserved.Reproduction or disclosure of this file or its contents without the prior
        written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		ImGuiPerformance.h
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		23 September 2023
@brief		This file contains the functions declaration for performance window in ImGui

*//*______________________________________________________________________*/
#pragma once
#include "ImGuiLib.h"
#include "DebugProfile.h"

void InitPerformance(GLFWwindow* window);

void UpdatePerformance(GLFWwindow* window);