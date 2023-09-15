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

Logger() : Constructor
~Logger() : Destructor
log() : Log into the file
rotateLogFile() : Rotate the log file
setLevel() : Set the log level
getLevel() : Get the log level
getTimeStamp() : Get the time stamp
getLogFileSize() : Get the log file size


TODO :
Maybe rotating of log file between 3 log files
Delete the files by time and number of log file created

*//*______________________________________________________________________*/

#include "DebugLog.h"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include "vMath.h"
#include "debugdiagnostic.h"
#include <filesystem>


constexpr size_t MAX_FILE_SIZE {1024 * 1024}; // 1MB

namespace debuglog {

	// Variables
	HANDLE hConsole;

	// Set the text colour to white
	WORD textColour = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;

	// Constructor
	Logger::Logger() {
		currentLogFileName = "testlog.txt";
		this->currentLogLevel = LOG_LEVEL::Trace;
		this->loggingEnabled = true;

		// Open the file
		logFile.open(currentLogFileName, std::ios::out | std::ios::app);

		// If logfile cannot open for some reason
		if (!logFile) {
			Assert(!logFile, "Error opening file");
			exit(1);
		}
	}

	// Logger
	Logger::Logger(const std::string& logFileName, LOG_LEVEL level, bool loggingEnabled) {

		currentLogFileName = logFileName;
		this->currentLogLevel = level;
		this->loggingEnabled = loggingEnabled;

		// Open the file
		logFile.open(logFileName, std::ios::out | std::ios::app);

		// If logfile cannot open for some reason
		if (!logFile) {
			Assert(!logFile, "Error opening file");
			exit(1);
		}
	}

	// Destructor
	Logger::~Logger() {

		// Close the file
		logFile.close();
	}

	// Log into the file
	void Logger::log(LOG_LEVEL level, const std::string& message) {

		// Change the colour of the console
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (level) {
		case LOG_LEVEL::Trace:
			// Change the console colour
			SetConsoleTextAttribute(hConsole, textColour | FOREGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
			break;

		case LOG_LEVEL::Debug:
			// Change the console colour
			SetConsoleTextAttribute(hConsole, textColour | FOREGROUND_INTENSITY | BACKGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_INTENSITY);
			break;

		case LOG_LEVEL::Info:
			// Change the console colour
			SetConsoleTextAttribute(hConsole, textColour | FOREGROUND_INTENSITY | BACKGROUND_GREEN);
			break;

		case LOG_LEVEL::Warning:
			// Change the console colour
			SetConsoleTextAttribute(hConsole, textColour | FOREGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_RED);
			break;

		case LOG_LEVEL::Error:
			// Change the console colour
			SetConsoleTextAttribute(hConsole, textColour | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY);
			break;

		case LOG_LEVEL::Fatal:
			// Change the console colour
			SetConsoleTextAttribute(hConsole, textColour | FOREGROUND_INTENSITY | BACKGROUND_RED);
			break;
		}

		// If the logging is enabled and current log level is lower than the level set
		if (loggingEnabled && static_cast<int>(level) >= static_cast<int>(currentLogLevel)) {

			// Get the time
			std::string timeStamp = getTimeStamp();

			// Get the current level
			std::string levels = getLevel(level);

			logFile << timeStamp << " [" << levels << "] " << message << "\n";
			std::cout << timeStamp << " [" << levels << "] " << message << "\n";
		}

		// Change back to default colour
		SetConsoleTextAttribute(hConsole, textColour );

		rotateLogFile(MAX_FILE_SIZE);
	}


	void Logger::trace(const std::string&message) {

		// Call the log function with TRACE log level
		log(LOG_LEVEL::Trace, message);
	}

	void Logger::debug(const std::string &message) {

		// Call the log function with DEBUG log level
		log(LOG_LEVEL::Debug, message);
	}

	void Logger::info(const std::string &message) {

		// Call the log function with INFO log level
		log(LOG_LEVEL::Info, message);
	}

	void Logger::warning(const std::string &message) {

		// Call the log function with WARNING log level
		log(LOG_LEVEL::Warning, message);
	}

	void Logger::error(const std::string &message) {

		// Call the log function with ERROR log level
		log(LOG_LEVEL::Error, message);
	}

	void Logger::fatal(const std::string &message) {

		// Call the log function with FATAL log level
		log(LOG_LEVEL::Fatal, message);
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
				Assert(!(std::rename(currentLogFileName.c_str(), newFileName.c_str()) != 0), "Cannot rename file");
			}



			// Reopen the log file
			logFile.open(currentLogFileName, std::ios::out | std::ios::app);
			if (!logFile.is_open()) {
				Assert(!logFile, "Cannot open file");
			}
		}
	}
	
	// Set the log level
	void Logger::setLevel(LOG_LEVEL level) {
		currentLogLevel = level;
	}

	// Set the logging enabling
	void Logger::setLoggingEnabled(bool toggle) {
		loggingEnabled = toggle;
	}

	// Get the log level
	std::string Logger::getLevel(LOG_LEVEL level) {
		switch (level) {

		case LOG_LEVEL::Trace:
			return "T";
		case LOG_LEVEL::Debug:
			return "D";
		case LOG_LEVEL::Info:
			return "I";
		case LOG_LEVEL::Warning:
			return "W";
		case LOG_LEVEL::Error:
			return "E";
		case LOG_LEVEL::Fatal:
			return "F";
		default:
			return "U";
		}
	}

	// Get the timestamp
	std::string Logger::getTimeStamp(void) {
		std::time_t now = std::time(nullptr);
		struct tm timeInfo;
		localtime_s(&timeInfo, &now);
		char timestamp[20];
		std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeInfo);
		return std::string(timestamp);
	}

	// Get log file size
	std::streampos Logger::getLogFileSize(void) {
		logFile.seekp(0, std::ios::end);
		return logFile.tellp();
	}

	// Get the toggling enable
	bool Logger::getLoggingEnabled(void) {
		return loggingEnabled;
	}


	// For debugging
	Logger logger("test.log", debuglog::LOG_LEVEL::Trace, ENABLE_DEBUG_DIAG);
}

