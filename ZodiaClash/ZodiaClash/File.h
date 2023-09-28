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

/*!***********************************************************************
 \brief
	Gets the file extension e.g. .jpg, .ttf, .mp3
 \param [in] filePath
 \return
	string of the extenstion
 *************************************************************************/
	static std::string GetFileExtension(const std::string& filePath);
/*!***********************************************************************
	\brief
	One stop function to create filepath for all file types,
	just input filename and file type. 
	E.g. FileName = Characters, FileType = CSV
	build would be Characters.csv in the /CSV folder
	\param [in] FileName
	\param [in] FileType
	CSV,Frags,Vert,Textures
	\return
	a built file path e.g. ../Assets/CSV/Characters.csv
*************************************************************************/
	std::string FilePathDir(std::string fileName, FileType fileType);

	std::string Extension;
	std::string FileName;
	std::string FullDirectory;
	std::string FullPath;
};
