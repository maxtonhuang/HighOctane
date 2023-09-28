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
*	@file		File.cpp
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
*	@date		6 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains all the definitions for File I/O functions
******************************************************************************/
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

std::string FilePath::GetFileExtension(const std::string& filePath) {
	size_t dotPos = filePath.find_last_of(".");
	if (dotPos != std::string::npos) {
		return filePath.substr(dotPos);
	}
	return ""; // If no dot (.) is found, return an empty string to indicate no extension.
}

std::string FilePath::FilePathDir(std::string fileName, FileType fileType) {
	std::string retFile{"../Assets/"};
	
	switch (fileType) {
	case FileType::CSV:
		retFile += "CSV/" + fileName + ".csv";
		return retFile;
		break;
	case FileType::Vert:
		retFile += "Shaders/" + fileName + ".vert";
		return retFile;
		break;
	case FileType::Frag:
		retFile += "Shaders/" + fileName + ".frag";
		return retFile;
		break;
	case FileType::Textures:
		retFile += "Textures/" + fileName + ".png";
		return retFile;
		break;
	}
	return retFile;
}
