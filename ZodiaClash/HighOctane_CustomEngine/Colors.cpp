#include "Colors.h"
#include "debugdiagnostic.h"
#include "Serialization.h"
#include "AssetManager.h"

ColorManager colors;

void ColorManager::ReadColors() {
    const std::string filePath = "colors.txt";
    std::string path{ filePath };
    Serializer serializer;
    std::string parentDir = assetmanager.GetDefaultPath();
    path = parentDir + filePath;
    if (!serializer.Open(path)) {
        //ASSERT(1, "Unable to initialize color!");
        return;
    }
    /*
    if (!serializer.Open(path)) {
        parentDir = "../Assets/";
        path = parentDir + filePath;
        if (!serializer.Open(path)) {
            ASSERT(1, "Unable to initialize color!");
            return;
        }
    }
    */

    std::string line;
    std::string variableName;
    float r, g, b, a;
    while (getline(serializer.stream, line)) {
        std::istringstream iss(line);
        iss >> variableName >> r >> g >> b >> a;
        //DEBUG_PRINT("%s : %f %f %f %f", variableName.c_str(), r, g, b, a);
        glm::vec4 newColor(r, g, b, a);
        colorMap[variableName] = newColor;
    }
}

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

void ColorManager::UpdateColorsFile() {
    const std::string filePath = "colors.txt";
    std::string path{ filePath };
    std::string parentDir = "../Assets/";
    path = parentDir + filePath;
    std::ofstream colorsFile(path);
    if (!colorsFile.is_open()) {
        //ASSERT(1, "Unable to open colors file for writing!");
        return;
    }

    // Iterate through the colorMap and rewrite colors.txt
    for (std::pair<const std::string, glm::vec4>& entry : colorMap) {
        colorsFile << entry.first << " " << entry.second.r << " " << entry.second.g << " " << entry.second.b << " " << entry.second.a << std::endl;
    }
    colorsFile.close();
    //DEBUG_PRINT("updated color preset!");
}