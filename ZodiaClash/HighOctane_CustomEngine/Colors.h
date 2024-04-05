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
*	@file		Colors.h
*
*	@author		Chua Zhen Rong
*
*	@email		c.zhenrong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		26 February 2024
*
* *****************************************************************************
*
*	@brief
*
*
******************************************************************************/
#pragma once
#include "GraphLib.h"
#include <unordered_map>
#include <string>

class ColorManager {
public:
	std::unordered_map<std::string, glm::vec4> colorMap;

	//initialize in AssetManager
	void ReadColors();

	//update color presets / color styles
	void UpdateColor(const std::string& colorName, const glm::vec4& newColor);
	void UpdateColorsFile();
};

extern ColorManager colors;