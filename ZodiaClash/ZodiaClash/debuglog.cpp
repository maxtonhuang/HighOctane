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
RotateLogFile() : Rotate the log file
SetLevel() : Set the log level
GetLevel() : Get the log level
GetTimeStamp() : Get the time stamp
GetLogFileSize() : Get the log file size


TODO :
Maybe rotating of log file between 3 log files
Delete the files by time and number of log file created


WHEN IMGUI IS DONE
Dynamically change the log level during run time
*//*______________________________________________________________________*/

#include "DebugLog.h"
#include "GUIManager.h"



namespace debuglog {

	// Constructor
	Logger::Logger() {
		currentLogFileName = "console.log";
		this->currentLogLevel = LOG_LEVEL::Trace;

		// Open the file
		logFile.open(currentLogFileName, std::ios::out | std::ios::app);

		// If logfile cannot open for some reason
		if (!logFile) {
			throw std::runtime_error("File cannot be opened");
		}
	}

	// Logger
	Logger::Logger(const std::string& logFileName, LOG_LEVEL level) {

		currentLogFileName = logFileName;
		this->currentLogLevel = level;

		// Open the file
		logFile.open(logFileName, std::ios::out | std::ios::app);

		// If logfile cannot open for some reason
		if (!logFile) {
			throw std::runtime_error("File cannot be opened");
		}
	}

	// Destructor
	Logger::~Logger() {

		// Close the file
		logFile.close();
	}

	// Log into the file
	void Logger::log(LOG_LEVEL level, const std::string& message) {

		// If the logging is enabled and current log level is lower than the level set
		if (static_cast<int>(level) >= static_cast<int>(currentLogLevel)) {

			// Get the time
			std::string timeStamp = GetTimeStamp();

			// Get the current level
			std::string levels = GetLevel(level);

			// Write to the console
			std::cout << timeStamp << " [" << levels << "] " << message << std::endl;
		}

		//ROTATELOGFILEL(MAX_FILE_SIZE);
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
	void Logger::RotateLogFile(size_t maxFileSize) {
		// Check if the log file needs rotation based on the maximum file size
		if (static_cast<size_t>(GetLogFileSize()) >= maxFileSize) {
			// Close the current log file
			logFile.close();

            std::filesystem::path currentPath(currentLogFileName);
            std::filesystem::path newPath = currentPath.stem();

            newPath += "Old.log";

			// Rename the file using std::filesystem
			std::filesystem::rename(currentPath, newPath);

			// Reopen the log file
			logFile.open(currentLogFileName, std::ios::out | std::ios::app);
			if (!logFile.is_open()) {
				throw std::runtime_error("File cannot be opened");
			}

			// Remove the old file
			//std::filesystem::remove(newPath);
		}
	}
	
	// Set the log level
	void Logger::SetLevel(LOG_LEVEL level) {
		currentLogLevel = level;
	}

	// Get the log level
	std::string Logger::GetLevel(LOG_LEVEL level) {
		switch (level) {

		case LOG_LEVEL::Trace:
			return "Trace";
		case LOG_LEVEL::Debug:
			return "Debug";
		case LOG_LEVEL::Info:
			return "Info";
		case LOG_LEVEL::Warning:
			return "Warning";
		case LOG_LEVEL::Error:
			return "Error";
		case LOG_LEVEL::Fatal:
			return "Fatal";
		default:
			return "Unknown";
		}
	}
	
	// Get the timestamp
	std::string Logger::GetTimeStamp(void) {
		
		// Get the current time
		std::time_t now = std::time(nullptr);

		// Get the time info
		struct tm timeInfo;
		localtime_s(&timeInfo, &now);
		char timestamp[20];

		// Format the time
		std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeInfo);

		// Return the timestamp
		return std::string(timestamp);
	}

	// Get log file size
	std::streampos Logger::GetLogFileSize() {
		return std::filesystem::file_size(currentLogFileName);
	}


	#if ENABLE_DEBUG_DIAG
		// For debugging
	Logger logger;
	#endif
}

