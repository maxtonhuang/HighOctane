#pragma once
#include "ECS.h"

using Event = void(*)(std::string);



class EventManager {
public:
	void Call(std::string functionName, std::string functionInput);
	void InitialiseFunctions();
	std::vector<const char*> GetFunctionNames();
private:
	std::unordered_map<std::string, Event> functions;
	std::vector<const char*> functionNames;
};

extern EventManager events;