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
*	@file		Serialization.h
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		10 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains all the declarations for serializer class
*	
*	!--Future Updates--!
*	
*	-M1 Checkpoint-
*	- Attempt at creating a CSV parser
*	- Create a Json to CSV, Vice versa
*	- Find better optimization techniques for serialization
******************************************************************************/
#pragma once
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include "debugdiagnostic.h"
#include "EntityFactory.h"
#include "Components.h"
#include "ECS.h"
#include "Message.h"
#include "EngineCore.h"
#include "VMath.h"
#include "GraphLib.h"
#include "Texture.h"

class Serializer {
public:
	std::ifstream stream;
	bool Open(const std::string& file);
	bool IsGood();
	void ReadInt(int& i);
	void ReadFloat(float& f);
	void ReadString(std::string& str);
	static void SerializeCSV(const std::string& file);
	Serializer& operator>>( int i) {
		stream >> i;
		return *this;
	};
	Serializer& operator>>( float f) {
		stream >> f;
		return *this;
	};
	Serializer& operator>>(std::string str) {
		stream >> str;
		return *this;
	};
	Serializer& operator>>(char c) {
		stream >> c;
		return *this;
	};
	static void SaveEntityToJson(const std::string& fileName, const Entity& entity);
	static bool LoadEntityFromJson(const std::string& fileName);

};


/***********************************Testing for CSV Parser*******************************************/
class PseudoCharacter {
public:
	int id{};
	std::string name{};
	float health{};
	float attack{};
	float defence{};
	float speed{};
};

enum class CharacterComponents {
	COLOR,
	TRANSFORM,
	TEXTURE,
	MATRIX,
	VISIBLE,
	SIZE,
	MAINCHARACTER
};
