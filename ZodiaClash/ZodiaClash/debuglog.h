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
*	@file		DebugLog.h
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
*	@brief		This file contains the functions declaration for debugging
*
*   1. Include this header file in the file you want to use the functions
*
*	2.	\code{.cpp}
*			LOG_INFO("Program Started");
*		\endcode
*
*	This will print out the following message to the custom console:
*	"YYYY-MM-DD HH:MM:SS [I] Program started"
******************************************************************************/
#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include "VMath.h"
#include <chrono>	
#include <ctime>
#include <filesystem>
#include <exception>


// Move this together with the bottom code please thank you to either engine or graphics
#include "Framework.h"

// Define this to enable debug diagnostics
// In debug mode, the logging tools will run
// In release mode, it won't
#if !defined(ENABLE_DEBUG_DIAG) 
#   if defined(_DEBUG)
#       define ENABLE_DEBUG_DIAG 1

#   else
#       define ENABLE_DEBUG_DIAG 0
#   endif
#endif

constexpr size_t MAX_FILE_SIZE{ 1024 * 1024 }; // 1MB

// If debug diagnostics is enabled, then we will use the debug printing function
#if ENABLE_DEBUG_DIAG

#define LOG_TRACE(message) debuglog::logger.trace(message);

#define LOG_DEBUG(message) debuglog::logger.debug(message);

#define LOG_INFO(message) debuglog::logger.info(message);

#define LOG_WARNING(message) debuglog::logger.warning(message);

#define LOG_ERROR(message) debuglog::logger.error(message);

#define LOG_FATAL(message) debuglog::logger.fatal(message);

#define LOG_ROTATE_FILE(maxFileSize) debuglog::logger.RotateLogFile(maxFileSize);

#define LOG_CRASH(message) debuglog::crashLogger.CrashLog(debuglog::LOG_LEVEL::Fatal, message);
 
#define LOG_SET_LEVEL(level) debuglog::logger.SetLevel(level);

#define LOG_GET_LEVEL(level) debuglog::logger.GetLevel(level);



// Else, we will just ignore the debug printing function
#else
#define LOG_TRACE(message) ((void)0);

#define LOG_DEBUG(message) ((void)0);

#define LOG_INFO(message) ((void)0);

#define LOG_WARNING(message) ((void)0);

#define LOG_ERROR(message) ((void)0);

#define LOG_FATAL(message) ((void)0);

#define LOG_CRASH(message) ((void)0);

#define LOG_ROTATE_FILE(maxFileSize) ((void)0);

#define LOG_SET_LEVEL(level) ((void)0);

#define LOG_GET_LEVEL(level) ((void)0);
#endif

namespace debuglog {

	enum class LOG_LEVEL {
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	class Logger {
		public:
			Logger();
			Logger(const std::string &logFileName, LOG_LEVEL level = LOG_LEVEL::Trace);
			~Logger();
			
			void trace(const std::string& message);
			void debug(const std::string& message);
			void info(const std::string& message);
			void warning(const std::string &message);
			void error(const std::string &message);
			void fatal(const std::string &message);

			void RotateLogFile(size_t maxFileSize);

			void SetLevel(LOG_LEVEL level);

			std::string GetLevel(LOG_LEVEL level);
			std::string GetTimeStamp();
			std::streampos GetLogFileSize();
			void log(LOG_LEVEL level, const std::string& message);
			void CrashLog(LOG_LEVEL level, const std::string& message);
		private:

			std::ofstream logFile;
			std::string currentLogFileName;
			LOG_LEVEL currentLogLevel;
	};

	// Extern declarations
	extern Logger logger;
	extern Logger crashLogger;
}

