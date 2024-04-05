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

/*!***********************************************************************
 \brief
  Converts the forward slash characters to backslash characters in a given character and converts uppercase characters to lowercase.
 \param d
  A character that will be checked and potentially converted.
 \return
  Returns the converted character. If the input character is a forward slash ('/'), it returns a backslash ('\\'). For any other character, it returns the lowercase version of the character.
 *************************************************************************/

char convertCharacters(char d)
{
	if (d == '/')
		return '\\';
	else
		return (char)tolower(d);
}

/*!***********************************************************************
 \brief
  Converts all uppercase characters in a given string to lowercase and changes forward slashes to backslashes.
 \param s
  A reference to the string that will be modified in place.
 \return
  This function does not return a value.
 *************************************************************************/

void toLower(std::string& s) {
	std::transform(s.begin(), s.end(), s.begin(), convertCharacters);
}

/*!***********************************************************************
 \brief
  Sets the file path for an object of the `FilePath` class, processing the given string to extract and store the file name, file extension, and the full directory path. It also converts the file path to lowercase and replaces forward slashes with backslashes.
 \param file
  A string representing the file path to be processed and set. This parameter is modified during the execution of the function to aid in extracting the file name, extension, and directory path.
 \return
  This function does not return a value. It sets the `FullPath`, `FileName`, `Extension`, and `FullDirectory` member variables of the `FilePath` class based on the processed input.
 *************************************************************************/

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

/*!***********************************************************************
 \brief
  Constructs a new file path string for the current file by appending a new file extension to the original file path, excluding the original file extension.
 \param newExtension
  A constant reference to a string representing the new file extension to be appended to the file path. This should include the dot ('.') if it's part of the desired extension format.
 \return
  Returns a string that represents the file path with the new extension. This new file path consists of the original full directory path, the file name without its original extension, and the new extension appended.
 *************************************************************************/
std::string FilePath::GetFilePathWithNewExtension(const std::string& newExtension)
{
	return FullDirectory + FileName + newExtension;
}

/*!***********************************************************************
 \brief
	Gets the file extension e.g. .jpg, .ttf, .mp3
 \param [in] filePath
 \return
	string of the extenstion
 *************************************************************************/
std::string FilePath::GetFileExtension(const std::string& filePath) {
	size_t dotPos = filePath.find_last_of(".");
	if (dotPos != std::string::npos) {
		return filePath.substr(dotPos);
	}
	return ""; // If no dot (.) is found, return an empty string to indicate no extension.
}

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
