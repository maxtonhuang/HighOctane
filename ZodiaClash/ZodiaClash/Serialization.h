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
	static void SerializeBuild(const std::string& file);
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
