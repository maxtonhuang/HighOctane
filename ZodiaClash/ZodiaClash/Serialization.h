#pragma once
#include <fstream>
#include <istream>
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
};