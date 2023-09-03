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
#include "vmath.h"
#include <chrono>

// Define this to enable debug diagnostics
// In debug mode, the logging tools will run
// In release mode, it won't
#if !defined(G_ENABLE_DEBUG_DIAGNOSTICS) 
#   if defined(_DEBUG)
#       define G_ENABLE_DEBUG_DIAGNOSTICS 1
#   else
#       define G_ENABLE_DEBUG_DIAGNOSTICS 0
#   endif
#endif

extern int gEnableDebugDiagnostics;

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
			Logger(const std::string &logFileName = "testlog.txt", LOG_LEVEL level = LOG_LEVEL::Trace, bool loggingEnabled = true);
			~Logger();

			
			void trace(const std::string& message);
			void debug(const std::string& message);
			void info(const std::string& message);
			void warning(const std::string &message);
			void error(const std::string &message);
			void fatal(const std::string &message);


			void rotateLogFile(size_t maxFileSize);

			void setLevel(LOG_LEVEL level);
			void setLoggingEnabled(bool toggle);

			std::string getLevel(LOG_LEVEL level);
			std::string getTimeStamp();
			std::streampos getLogFileSize();
			bool getLoggingEnabled();

		private:

			void log(LOG_LEVEL level, const std::string& message);

			std::ofstream logFile;
			std::string currentLogFileName;
			LOG_LEVEL currentLogLevel;

			bool loggingEnabled;



	};

	//Implement these functions to control how errors and
	//debug printing are handled
	//bool SignalErrorHandler(const char* expression, const char* file, int line, const char* formatMessage = 0, ...);
	//void DebugPrintHandler(const char* msg, ...);

	extern Logger logger;
}
