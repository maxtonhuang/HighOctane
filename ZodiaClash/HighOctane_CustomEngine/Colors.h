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