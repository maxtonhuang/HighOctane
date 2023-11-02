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
*	@file		Reflections.h
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
*	@brief		header files for the reflection library
*
*	This file contains the declaration of the functions for the reflection library
******************************************************************************/
#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <functional>

struct VariableInfo {
	std::string name;
	void* valuePtr;
	std::function<std::string()> read;
	std::function<void(const std::string&)> write;

	template<typename T>
	VariableInfo(const std::string& n, T* value)
		: name(n), valuePtr(value),
		read([value] { return variable_to_string(*value); }),
		write([value](const std::string& val) { *value = string_to_variable<T>(val); }) {}

	VariableInfo(const std::string& name, void* valuePtr, std::function<std::string()> read)
		: name(name), valuePtr(valuePtr), read(read) {}

	// Additional helper function to convert string to variable
	template<typename T>
	static T string_to_variable(const std::string& val);
};


extern std::vector<VariableInfo> variablesTEST;

// Helper template to define string_to_variable conversions
template<typename T>
T VariableInfo::string_to_variable(const std::string& val) {
	if constexpr (std::is_same_v<T, int>) {
		return std::stoi(val);
	}
	else if constexpr (std::is_same_v<T, float>) {
		return std::stof(val);
	}
	else if constexpr (std::is_same_v<T, std::string>) {
		return val;
	}
	else if constexpr (std::is_same_v<T, bool>) {
		return val == "true";
	}
	else {
		return T();
	}
}

// Helper templates to define to_string conversions
template<typename T>
std::string variable_to_string(T value) {
	if constexpr (std::is_same_v<T, int>) {
		return std::to_string(value);
	}
	else if constexpr (std::is_same_v<T, float>) {
		return std::to_string(value);
	}
	else if constexpr (std::is_same_v<T, std::string>) {
		return value;
	}
	else if constexpr (std::is_same_v<T, bool>) {
		return value ? "true" : "false";
	}
	else {
		return "<Unsupported type>";
	}
}


// Macro to convert variable to string based on its type
#define VAR_TO_STRING(var) variable_to_string(var)

// Macro to convert string to variable based on its type
#define STRING_TO_VAR(var, val) var = VariableInfo::string_to_variable<decltype(var)>(val)

// Macro to declare a variable and register it
#define DECLARE(TYPE, NAME, VALUE) \
    TYPE NAME = VALUE; \
    variablesTEST.emplace_back(#NAME, &NAME, [&NAME]() -> std::string { return VAR_TO_STRING(NAME); });
