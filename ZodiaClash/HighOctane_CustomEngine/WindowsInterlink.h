#pragma once

#include <vector>
#include <string>
#include "graphlib.h"

std::vector<std::string> OpenFileDialog();

void FileDropCallback(GLFWwindow* window, int count, const char** paths);

std::string SaveFileDialog();