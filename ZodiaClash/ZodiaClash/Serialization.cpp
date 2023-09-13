#include "Serialization.h"

bool Serializer::Open(const std::string& file)
{
	stream.open(file.c_str());
	return stream.is_open();
}

bool Serializer::IsGood()
{
	return stream.good();
}

void Serializer::ReadInt(int& i)
{
	stream >> i;
}

void Serializer::ReadFloat(float& f)
{
	stream >> f;
}

void Serializer::ReadString(std::string& str)
{
	stream >> str;
}

//template<typename T>
//void StreamRead(Serializer& stream, T& typeInstance)
//{
//	typeInstance.Serialize(stream);
//}


void StreamRead(Serializer& stream, float& f)
{
	stream.ReadFloat(f);
}

void StreamRead(Serializer& stream, int& i)
{
	stream.ReadInt(i);
}

void StreamRead(Serializer& stream, std::string& str)
{
	stream.ReadString(str);
}

void Serializer::SerializeCSV(const std::string& file) {
	Serializer stream;
	bool isFileOpen = stream.Open(file);
	Assert(!isFileOpen, "Unable to open file");
	std::string line, header;
	PseudoCharacter characterStats;
	std::getline(stream.stream, header);  // Read the header line and discard it
	char c;
	std::string field;

	while (!stream.stream.eof()) {
		field.clear();
		// Read characters until a ',' or newline is encountered
		while (stream.stream.get(c) && c != ',' && c != '\n') {
			field += c;
		}

		// Process the field based on the context (e.g., assign to the appropriate member)
		if (!field.empty()) {
			if (characterStats.id == 0) {
				characterStats.id = std::stoi(field);
			}
			else if (characterStats.name.empty()) {
				characterStats.name = field;
			}
			else if (characterStats.health == 0.0f) {
				characterStats.health = std::stof(field);
			}
			else if (characterStats.attack == 0.0f) {
				characterStats.attack = std::stof(field);
			}
			else if (characterStats.defence == 0.0f) {
				characterStats.defence = std::stof(field);
			}
			else if (characterStats.speed == 0.0f) {
				characterStats.speed = std::stof(field);
			}
		}
		//DebugPrint("ID:%d Name:%s  Health:%f  Attack:%f  Defence:%f  Speed:%f", characterStats.id, characterStats.name.c_str(), characterStats.health, characterStats.attack, characterStats.defence, characterStats.speed);

		// Check for newline to indicate the end of a line
		if (c == '\n' || stream.stream.eof()) {
			// Reset characterStats for the next line
			characterStats = PseudoCharacter();
		}
	}
}
