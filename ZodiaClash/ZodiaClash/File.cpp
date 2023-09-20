#include "File.h"
#include <algorithm>
char convertCharacters(char d)
{
	if (d == '/')
		return '\\';
	else
		return (char)tolower(d);
}

void toLower(std::string& s) {
	std::transform(s.begin(), s.end(), s.begin(), convertCharacters);
}

void FilePath::SetFilePath(std::string file)
{
	toLower(file);

	FullPath = file;

	std::size_t dirEnd = file.find_last_of("\\/");
	dirEnd = (dirEnd == std::string::npos) ? 0 : dirEnd + 1;

	std::size_t fileEnd = file.find_last_of(".");
	fileEnd = (fileEnd == std::string::npos) ? file.size() : fileEnd;

	//Extension may be nothing
	Extension = file.substr(fileEnd);
	FileName = file.substr(dirEnd, fileEnd - dirEnd);
	FullDirectory = file.substr(0, dirEnd);
}

std::string FilePath::GetFilePathWithNewExtension(const std::string& newExtension)
{
	return FullDirectory + FileName + newExtension;
}

std::string FilePath::FilePathDir(std::string fileName, FileType fileType) {
	std::string retFile{"../Assets/"};
	
	switch (fileType) {
	case FileType::CSV:
		retFile += "CSV/" + fileName + ".csv";
		return retFile;

	case FileType::Vert:
		retFile += "Shaders/" + fileName + ".vert";
		return retFile;

	case FileType::Frag:
		retFile += "Shaders/" + fileName + ".frag";
		return retFile;

	case FileType::Textures:
		retFile += "Textures/" + fileName + ".png";
		return retFile;

	}

}