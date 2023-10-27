#pragma once
#include "ECS.h"

using Event = void(*)(std::string);

void InitialiseFunctions();

extern std::unordered_map <std::string,Event> functions;