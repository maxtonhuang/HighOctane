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

template<typename T>
void StreamRead(Serializer& stream, T& typeInstance)
{
	typeInstance.Serialize(stream);
}


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

void SerializeCSV(const std::string& file) {
	Serializer stream;
	bool isFIleOpen = stream.Open(file);
	Assert(!isFIleOpen, "Unable to open file");
}
