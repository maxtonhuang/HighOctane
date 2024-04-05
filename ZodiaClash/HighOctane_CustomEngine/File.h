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
*	@file		File.h
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
*	This file contains all the declarations for File I/O functions
******************************************************************************/
#pragma once
#include <string>
#include <cctype>

enum class FileType {
	CSV,
	Frag,
	Vert,
	Textures
};

class FilePath {
public:
	FilePath() {};
	FilePath(std::string file) { SetFilePath(file); };
	FilePath& operator=(std::string file) {
		SetFilePath(file);
		return *this;
	};
	void SetFilePath(std::string file);
	std::string GetFilePathWithNewExtension(const std::string& newExtension);
	static std::string GetFileExtension(const std::string& filePath);
	std::string FilePathDir(std::string fileName, FileType fileType);

	std::string Extension;
	std::string FileName;
	std::string FullDirectory;
	std::string FullPath;
};
