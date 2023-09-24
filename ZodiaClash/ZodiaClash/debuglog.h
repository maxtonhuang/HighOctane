/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		debug.h
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		30 August 2023
@brief		This file contains the functions declaration for debugging
*//*______________________________________________________________________*/
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
			Logger(const std::string &logFileName = "testlog.txt", LOG_LEVEL level = LOG_LEVEL::Trace);
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

		private:



			std::ofstream logFile;
			std::string currentLogFileName;
			LOG_LEVEL currentLogLevel;
	};

	// Extern declarations
	extern Logger logger;
}

// If debug diagnostics is enabled, then we will use the debug printing function
#if ENABLE_DEBUG_DIAG

#define TRACEL(message) debuglog::logger.trace(message);

#define DEBUGL(message) debuglog::logger.debug(message);

#define INFOL(message) debuglog::logger.info(message);

#define WARNINGL(message) debuglog::logger.warning(message);

#define ERRORL(message) debuglog::logger.error(message);

#define FATALL(message) debuglog::logger.fatal(message);

#define ROTATELOGFILEL(maxFileSize) debuglog::logger.RotateLogFile(maxFileSize);

#define SETLEVELL(level) debuglog::logger.SetLevel(level);


// Else, we will just ignore the debug printing function
#else
#define TRACEL(message) ((void)0);

#define DEBUGL(message) ((void)0);

#define INFOL(message) ((void)0);

#define WARNINGL(message) ((void)0);

#define ERRORL(message) ((void)0);

#define FATALL(message) ((void)0);

#define ROTATELOGFILEL(maxFileSize) ((void)0);

#define SETLEVELL(level) ((void)0);

#endif