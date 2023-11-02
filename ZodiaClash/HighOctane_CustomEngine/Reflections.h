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
#include <fstream>
#include <sstream>

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

/*!
* \brief Converts a string to the corresponding variable type.
*
* \tparam T Type to convert the string into.
* \param val The string value to convert.
* \return The value of type T after conversion from the string.
*/
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

/*!
* \brief Converts a variable to a string.
*
* \tparam T Type of the variable to convert to string.
* \param value The variable to convert to a string.
* \return The variable represented as a string.
*/
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

/*!
* \brief Declares a variable and registers it into the system.
*
* \param TYPE The type of the variable to declare.
* \param NAME The name of the variable.
* \param VALUE The initial value of the variable.
*/
#define DECLARE(TYPE, NAME, VALUE) \
    TYPE NAME = VALUE; \
    variablesTEST.emplace_back(#NAME, &NAME, [&NAME]() -> std::string { return VAR_TO_STRING(NAME); });

/**
 * Serializes a collection of VariableInfo instances into a string.
 * @param variables A vector containing VariableInfo instances to serialize.
 * @return A string containing the serialized data.
 */
std::string SerializeVariables(const std::vector<VariableInfo>& variables);

/**
 * Deserializes a string to populate a vector of VariableInfo instances.
 * @param data A string containing serialized data to be deserialized.
 * @param variables A vector to store deserialized VariableInfo instances.
 */
void DeserializeVariables(const std::string& data, std::vector<VariableInfo>& variables);

/**
 * Serializes a collection of VariableInfo instances and writes the data to a file.
 * @param filename Name of the file to serialize the data into.
 * @param variables A vector containing VariableInfo instances to serialize.
 */
void SerializeVariablesToFile(const std::string& filename, const std::vector<VariableInfo>& variables);

/**
 * Reads serialized data from a file and populates a vector of VariableInfo instances.
 * @param filename Name of the file to read the serialized data from.
 * @param variables A vector to store deserialized VariableInfo instances.
 */
void DeserializeVariablesFromFile(const std::string& filename, std::vector<VariableInfo>& variables);
