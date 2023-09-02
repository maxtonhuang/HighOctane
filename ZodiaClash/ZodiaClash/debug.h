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

//By defining G_ENABLE_DEBUG_DIAGNOSTICS you can explicitly 
//enable or disable debugging and diagnostic macros
//#if !defined(G_ENABLE_DEBUG_DIAGNOSTICS) 
//#   if defined(_DEBUG)
//#       define G_ENABLE_DEBUG_DIAGNOSTICS 1
//#   else
//#       define G_ENABLE_DEBUG_DIAGNOSTICS 0
//#   endif
//#endif

namespace debuglog {

	class Logger {
		public:
			Logger(const std::string &logFileName = "testlog.txt");

			void log(const std::string &message);

			void rotateLogFile(size_t maxFileSize);
		private:
			std::ofstream logFile;
			std::string currentLogFileName;


			std::string getTimeStamp();
			std::streampos getLogFileSize();
	};

}
