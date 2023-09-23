#pragma once
#include <string>
#include <cctype>

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

	std::string Extension;
	std::string FileName;
	std::string FullDirectory;
	std::string FullPath;
};