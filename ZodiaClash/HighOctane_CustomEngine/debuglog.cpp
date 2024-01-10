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
*	@file		DebugLog.cpp
*
*	@author		Koh Wen Yuan
*
*	@email		k.wenyuan\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		3o August 2023
*
* *****************************************************************************
*
*	@brief		This file contains the functions definition for debugging
*
*	Logger() : Constructor
*	~Logger() : Destructor
*	log() : Log into the file
*   CrashLog() : Log into the crash file
*	RotateLogFile() : Rotate the log file
*	SetLevel() : Set the log level
*	GetLevel() : Get the log level
*	GetTimeStamp() : Get the time stamp
*	GetLogFileSize() : Get the log file size
*
******************************************************************************/

#include "DebugLog.h"
//#include "GUIManager.h"



namespace debuglog {

	    /*!
     * \brief Default constructor
     *
     * This function is the default constructor
     */
	Logger::Logger() {
		//currentLogFileName = "console.txt";
		//this->currentLogLevel = LOG_LEVEL::Trace;

		//// Open the file
		//logFile.open(currentLogFileName, std::ios::out | std::ios::app);

		//// If logfile cannot open for some reason
		//if (!logFile) {
		//	throw std::runtime_error("File cannot be opened");
		//}
	}

	    /*!
     * \brief Constructor
     *
     * This function is the other constructor
     */
	Logger::Logger(const std::string& logFileName, LOG_LEVEL level) {

		currentLogFileName = logFileName;
		this->currentLogLevel = level;

		//// Open the file
		//logFile.open(logFileName, std::ios::out | std::ios::app);

		//// If logfile cannot open for some reason
		//if (!logFile) {
		//	throw std::runtime_error("File cannot be opened");
		//}
	}

	 /*!
     * \brief Destructor
     *
     * This function is the destructor
     */
	Logger::~Logger() {

		// Close the file
		logFile.close();
	}

	/*!
     * \brief Logging function
     *
     * This function is responsible for the logging of the messages into the custom console
	 * It is now only logging into the custom console as in our engine, there is a choice on
	 * whether to user chooses to log into file
     *
     * \param kevel : The level of the message
	 * \param message : The message to be logged
     * 
     */
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

	/*!
	 * \brief Crash logging function
	 *
	 * This function is responsible for the logging of the crash messages automatically
	 * onto the file crash.log
	 *
	 * \param kevel : The level of the message
	 * \param message : The message to be logged
	 * 
     */
	//void Logger::CrashLog(LOG_LEVEL level, const std::string& message) {

	//	// If the logging is enabled and current log level is lower than the level set
	//	if (static_cast<int>(level) >= static_cast<int>(currentLogLevel)) {

	//		// Get the time
	//		std::string timeStamp = GetTimeStamp();

	//		// Get the current level
	//		std::string levels = GetLevel(level);

	//		// Write to the file crash.log
	//		std::ofstream crashFile("crash.log", std::ios::out | std::ios::app);
	//		if (!crashFile) {
	//			throw std::runtime_error("File cannot be opened");
	//		}
	//		crashFile << message << std::endl;
	//	}
	//}
	void Logger::CrashLog(LOG_LEVEL level, const std::string& message) {
		(void)message;
		// If the logging is enabled and current log level is lower than the level set
		if (static_cast<int>(level) >= static_cast<int>(currentLogLevel)) {

			// Get the time
			std::string timeStamp = GetTimeStamp();

			// Get the current level
			std::string levels = GetLevel(level);

			// Folder where the log file will be saved
			const std::string folderName = "Logs";

			// Create the Logs directory if it doesn't exist
			std::filesystem::create_directory(folderName);

			// Construct the full file name
			std::string fullFileName = folderName + "/crash.txt";

			// Write to the file crash.log
			//std::ofstream crashFile(fullFileName, std::ios::out | std::ios::app);
			//if (!crashFile) {
			//	throw std::runtime_error("File cannot be opened: " + fullFileName);
			//}
			//crashFile << "[" << timeStamp << "] [" << levels << "] " << message << std::endl;
		}
	}
	/*!
	 * \brief trace logging function
	 *
	 * This function is responsible for the logging of the messages into the custom console
	 *
	 * \param message : The message to be logged
	 * 
     */
	void Logger::trace(const std::string&message) {

		// Call the log function with TRACE log level
		log(LOG_LEVEL::Trace, message);
	}

	/*!
	 * \brief trace logging function
	 *
	 * This function is responsible for the logging of the messages into the custom console
	 *
	 * \param message : The message to be logged
	 * 
     */
	void Logger::debug(const std::string &message) {

		// Call the log function with DEBUG log level
		log(LOG_LEVEL::Debug, message);
	}

		/*!
	 * \brief trace logging function
	 *
	 * This function is responsible for the logging of the messages into the custom console
	 *
	 * \param message : The message to be logged
	 * 
     */
	void Logger::info(const std::string &message) {

		// Call the log function with INFO log level
		log(LOG_LEVEL::Info, message);
	}

		/*!
	 * \brief trace logging function
	 *
	 * This function is responsible for the logging of the messages into the custom console
	 *
	 * \param message : The message to be logged
	 * 
     */
	void Logger::warning(const std::string &message) {

		// Call the log function with WARNING log level
		log(LOG_LEVEL::Warning, message);
	}

		/*!
	 * \brief trace logging function
	 *
	 * This function is responsible for the logging of the messages into the custom console
	 *
	 * \param message : The message to be logged
	 * 
     */
	void Logger::error(const std::string &message) {

		// Call the log function with ERROR log level
		log(LOG_LEVEL::Error, message);
	}
		/*!
	 * \brief trace logging function
	 *
	 * This function is responsible for the logging of the messages into the custom console
	 *
	 * \param message : The message to be logged
	 * 
     */
	void Logger::fatal(const std::string &message) {

		// Call the log function with FATAL log level
		log(LOG_LEVEL::Fatal, message);
	}

		/*!
	 * \brief Rotation of log file
	 *
	 * This function is responsible for the rotation of the log file based on the maximum file size
	 * However it is not currently in used as we are not logging into the file automatically
	 *
	 * \param maxFileSize : The maximum file size
	 * 
     */
	void Logger::RotateLogFile(size_t maxFileSize) {
		// Check if the log file needs rotation based on the maximum file size
		if (static_cast<size_t>(GetLogFileSize()) >= maxFileSize) {
			// Close the current log file
			logFile.close();

            std::filesystem::path currentPath(currentLogFileName);
            std::filesystem::path newPath = currentPath.stem();

            newPath += "Old.txt";

			// Rename the file using std::filesystem
			std::filesystem::rename(currentPath, newPath);

			// Reopen the log file
			//logFile.open(currentLogFileName, std::ios::out | std::ios::app);
			//if (!logFile.is_open()) {
			//	throw std::runtime_error("File cannot be opened");
			//}

			// Remove the old file
			//std::filesystem::remove(newPath);
		}
	}

		/*!
	 * \brief Set the log level
	 *
	 * This function is responsible for setting the log level
	 *
	 * \param level : The log level
	 * 
     */
	void Logger::SetLevel(LOG_LEVEL level) {
		currentLogLevel = level;
	}


		/*!
	 * \brief Get the log level
	 *
	 * This function is responsible for the getting the log level
	 *
	 * \param level : The log level
	 * 
     */
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
	

		/*!
	 * \brief Get the time stamp
	 *
	 * This function is responsible for getting the time stamp to use
	 * in the logging funtions
	 *
	 * 
     */
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

		/*!
	 * \brief Get the log size
	 *
	 * This function is responsible for getting the log size
	 * 
     */
	std::streampos Logger::GetLogFileSize() {
		return std::filesystem::file_size(currentLogFileName);
	}


	//#if ENABLE_DEBUG_DIAG
		// For debugging
	Logger logger;
	Logger crashLogger;
	//#endif
}

