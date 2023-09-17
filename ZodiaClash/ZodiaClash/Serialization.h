#pragma once
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include "debugdiagnostic.h"

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