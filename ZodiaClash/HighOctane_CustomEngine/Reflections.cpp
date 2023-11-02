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
*	@file		Reflections.cpp
*
*	@author		Koh Wen Yuan
*
*	@email		k.wenyuan\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		20 October 2023
*
* *****************************************************************************
*
*	@brief		cpp files for the reflection library
*
*	This file contains the definition of the functions for the reflection library
******************************************************************************/
#include "Reflections.h"

std::vector<VariableInfo> variablesTEST{};


std::string SerializeVariables(const std::vector<VariableInfo>& variables) {
	std::stringstream ss;
	for (const auto& varInfo : variables) {
		ss << varInfo.name << "=" << varInfo.read() << "\n";
	}
	return ss.str();
}

void DeserializeVariables(const std::string& data, std::vector<VariableInfo>& variables) {
	std::istringstream ss(data);
	std::string line;

	std::unordered_map<std::string, VariableInfo*> varMap;
	for (auto& var : variables) {
		varMap[var.name] = &var;
	}

	while (std::getline(ss, line)) {
		auto delimiterPos = line.find('=');
		if (delimiterPos != std::string::npos) {
			std::string name = line.substr(0, delimiterPos);
			std::string value = line.substr(delimiterPos + 1);

			if (varMap.find(name) != varMap.end()) {
				varMap[name]->write(value);
			}
		}
	}
}

void SerializeVariablesToFile(const std::string& filename, const std::vector<VariableInfo>& variables) {
	std::ofstream file(filename);
	if (file.is_open()) {
		file << SerializeVariables(variables);
		file.close();
	}
	else {
		std::cerr << "Unable to open file for serialization: " << filename << std::endl;
	}
}


void DeserializeVariablesFromFile(const std::string& filename, std::vector<VariableInfo>& variables) {
	std::ifstream file(filename);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();
		DeserializeVariables(buffer.str(), variables);
	}
	else {
		std::cerr << "Unable to open file for deserialization: " << filename << std::endl;
	}
}