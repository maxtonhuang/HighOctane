/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		debug.cpp
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		30 August 2023
@brief		This file contains the functions definintion for debugging


TODO :
Maybe rotating of log file, now it only changes the file name to old

*//*______________________________________________________________________*/

#include "debug.h"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#define MAX_FILE_SIZE 1048576 // 1MB

namespace debuglog {

	// Logger
	Logger::Logger(const std::string &logFileName) {

		currentLogFileName = logFileName;

		// Open the file
		logFile.open(logFileName, std::ios::out | std::ios::app);
	
		// If logfile cannot open for some reason
		if (!logFile) {
			std::cerr << "Error opening file" << "\n";
			exit(1);
		}
	}

	// Log into the file
	void Logger::log(const std::string &message) {
		// Get the timestamp
		std::string timeStamp = getTimeStamp();
		// Write to file
		logFile << timeStamp << "\n" << message << "\n";

		rotateLogFile(MAX_FILE_SIZE);
	}

	// Rotate the log file
	void Logger::rotateLogFile(size_t maxFileSize) {
		// Check if the log file needs rotation based on the maximum file size
		if (static_cast<size_t>(getLogFileSize()) >= maxFileSize) {
			// Close the current log file
			logFile.close();

			std::string newFileName = currentLogFileName;
			newFileName.erase(newFileName.size() - 4); // Remove the .txt extension
			newFileName += "Old.txt"; 

			// If cannot rename
			if (std::rename(currentLogFileName.c_str(), newFileName.c_str()) != 0) {
				std::cerr << "Error: Failed to rename the log file." << std::endl;
			}

			// Reopen the log file
			logFile.open(currentLogFileName, std::ios::out | std::ios::app);
			if (!logFile.is_open()) {
				std::cerr << "Error: Unable to reopen the log file." << std::endl;
			}
		}
	}


	// Get the timestamp
	std::string Logger::getTimeStamp(void) {
		std::time_t now = std::time(nullptr);
		struct tm timeInfo;
		localtime_s(&timeInfo, &now); // Use localtime_s for safe conversion
		char timestamp[20];
		std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeInfo);
		return std::string(timestamp);
	}

	// Get log file size
	std::streampos Logger::getLogFileSize(void) {
		logFile.seekp(0, std::ios::end);
		return logFile.tellp();
	}
}

