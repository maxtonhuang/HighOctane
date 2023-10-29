#pragma once

#include <vector>
#include <string>
#include "graphlib.h"

void OpenFileDialog();

void FileDropCallback(GLFWwindow* window, int count, const char** paths);

std::string SaveFileDialog(std::string extensionName, std::string extensionDescription);

std::string OpenSingleFileDialog();