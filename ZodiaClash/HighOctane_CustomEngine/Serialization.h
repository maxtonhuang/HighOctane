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
#include <windows.h>
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
/*!***********************************************************************
 \brief
	Opens stream
 \return
	true if stream is open
 *************************************************************************/
	bool Open(const std::string& file);
/*!***********************************************************************
 \brief
	Checks if stream is good for serializer class
 \return
	true if good
 *************************************************************************/
	bool IsGood();
/*!***********************************************************************
 \brief
	Reading function for serializer class type !***To be used in the future***!
 \param [in] i
	int
 *************************************************************************/
	void ReadInt(int& i);
/*!***********************************************************************
 \brief
	Reading function for serializer class type !***To be used in the future***!
 \param [in] f
	float
 *************************************************************************/
	void ReadFloat(float& f);
/*!***********************************************************************
 \brief
	Reading function for serializer class type !***To be used in the future***!
 \param [in] str
	string
 *************************************************************************/
	void ReadString(std::string& str);

/*!***********************************************************************
 \brief
	Attempt at creating a csv parser !***Improvements to be made in the future***!
 \param [in] filename
	Filepath to load from
 \return
	CSV file succesfully loaded
 *************************************************************************/
	static void SerializeCSV(const std::string& file);

/*!***********************************************************************
 \brief
	>> operator for serializer class to be used by other appropriate seriliazer
	functions of same data tpye
 \param [in] i
	int
 *************************************************************************/
	Serializer& operator>>( int i) {
		stream >> i;
		return *this;
	};

/*!***********************************************************************
 \brief
	>> operator for serializer class to be used by other appropriate seriliazer 
	functions of same data tpye
 \param [in] f
	float
 *************************************************************************/
	Serializer& operator>>( float f) {
		stream >> f;
		return *this;
	};

/*!***********************************************************************
	\brief
	>> operator for serializer class to be used by other appropriate seriliazer
	functions of same data tpye
	\param [in] str
	string
*************************************************************************/
	Serializer& operator>>(std::string str) {
		stream >> str;
		return *this;
	};

/*!***********************************************************************
 \brief
	>> operator for serializer class to be used by other appropriate seriliazer 
	functions of same data tpye
 \param [in] c
	char
 *************************************************************************/
	Serializer& operator>>(char c) {
		stream >> c;
		return *this;
	};

/*!***********************************************************************
 \brief
	Json Serializer using RapidJson library
 \param [in] filename
	Filepath to save to "../Assets/Scenes/"filename".json
 \param [in] entity
	Entity or List of entities to save
 \return
	Saved json file to set path
 *************************************************************************/
	static void SaveEntityToJson(const std::string& fileName, const std::set<Entity>& entity);

/*!***********************************************************************
 \brief
	Json deserializer using RapidJson library
 \param [in] filename
	Filepath to load from "../Assets/Scenes/"filename".json
 \return
	Upon succesful loading of file would return true
 *************************************************************************/
	static bool LoadEntityFromJson(const std::string& fileName);

};

/*!***********************************************************************
 \brief
	Loads configuration file "Config.txt"
 \param [in] filename
	None
 \return
	None
 *************************************************************************/
void LoadConfig();


void WriteSpriteConfig(const char* filename, int rows, int cols);


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

//std::vector<std::string> OpenFileDialog();
std::string SaveFileDialog(std::string extensionName = std::string{}, std::string extensionDescription = std::string{});

//std::unordered_map<Entity, std::vector<std::string>> LoadScripting();
std::vector<std::string> LoadScripting(Entity par);