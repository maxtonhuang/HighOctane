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