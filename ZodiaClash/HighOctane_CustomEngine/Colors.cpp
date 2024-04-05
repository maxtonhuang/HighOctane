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
*	@file		Colors.cpp
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
*
******************************************************************************/
#include "Colors.h"
#include "debugdiagnostic.h"
#include "Serialization.h"
#include "AssetManager.h"

ColorManager colors;

/*!***********************************************************************
 \brief
  Reads color definitions from a text file and stores them in a map for later use. This method is designed to initialize the color manager with a predefined set of colors.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It populates the internal color map with colors defined in a text file located at a predetermined path.
 *************************************************************************/
void ColorManager::ReadColors() {
    const std::string filePath = "colors.txt";
    std::string path{ filePath };
    Serializer serializer;
    std::string parentDir = assetmanager.GetDefaultPath();
    path = parentDir + filePath;
    if (!serializer.Open(path)) {
        return;
    }
    std::string line;
    std::string variableName;
    float r, g, b, a;
    while (getline(serializer.stream, line)) {
        std::istringstream iss(line);
        iss >> variableName >> r >> g >> b >> a;
        glm::vec4 newColor(r, g, b, a);
        colorMap[variableName] = newColor;
    }
}

/*!***********************************************************************
 \brief
  Updates the color value of a specified color name in the color map. If the color name exists, its value is updated with the new color provided. This method also triggers the update of the colors definition file to reflect the changes made.
 \param colorName
  The name of the color to update.
 \param newColor
  The new color value to assign to the color name, given as a glm::vec4 containing the RGBA values.
 \return
  This method does not return a value. It updates the specified color's value in the internal map and the color definition file.
 *************************************************************************/
void ColorManager::UpdateColor(const std::string& colorName, const glm::vec4& newColor) {
    using colorMapType = std::unordered_multimap<std::string, glm::vec4>;
    colorMapType::iterator it = colorMap.find(colorName);
    if (it != colorMap.end()) {
        // directly updates entry with new color
        it->second = newColor;
        // saves updated color into textfile
        UpdateColorsFile();
    }
}

/*!***********************************************************************
 \brief
  Writes the current set of colors from the color map back to the colors definition file. This method is used to persist any changes made to the colors during runtime, ensuring that updated color values are saved.
 \param
  This method does not take any parameters.
 \return
  This method does not return a value. It overwrites the colors definition file with the current colors from the color map.
 *************************************************************************/
void ColorManager::UpdateColorsFile() {
    const std::string filePath = "colors.txt";
    std::string path{ filePath };
    std::string parentDir = "../Assets/";
    path = parentDir + filePath;
    std::ofstream colorsFile(path);
    if (!colorsFile.is_open()) {
        return;
    }

    // Iterate through the colorMap and rewrite colors.txt
    for (std::pair<const std::string, glm::vec4>& entry : colorMap) {
        colorsFile << entry.first << " " << entry.second.r << " " << entry.second.g << " " << entry.second.b << " " << entry.second.a << std::endl;
    }
    colorsFile.close();
}