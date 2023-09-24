/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		debugdiagnostic.h
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		30 August 2023
@brief		This file contains the functions declaration for debugging

HOW TO USE THE FUNCTIONS

1. Include this header file in the file you want to use the functions

2.	\code{.cpp}
		DebugPrint("Debug message: %s %d", "Value:", 42);
	\endcode

	This will print out the following message to the standard error stream:
	"Debug message: Value: 42"

3.	\code{.cpp}
		int x = -1;
		Assert(x > 0, "x must be more than 0");
	\endcode

	This will print out and (log to error file?) the following message to the standard error stream:
	Assertion failed in {code.cpp} {line }: x must be more than 0

TODO :
PUT THE PERFORMANCE() AT THE END OF THE GAME LOOP

*//*______________________________________________________________________*/
#pragma once

#include "debuglog.h"
#include <psapi.h>
#include <cstdio>
#include <cstdarg> // For va_list, va_start, va_end
#include <iostream>
#include <cstdlib> // For abort
#include <memory>
#include <stdlib.h>
#include <crtdbg.h>


// Do not need to touch this, it is auto
// If Debug mode, then enable debug diagnostics
// Else, disable it
#if !defined(ENABLE_DEBUG_DIAG) 
#   if defined(_DEBUG)
#       define ENABLE_DEBUG_DIAG 1

#   else
#       define ENABLE_DEBUG_DIAG 0
#   endif
#endif


// Define the path separator for different operating systems
#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif





namespace debug {

	// Is to print debug messages
	void PrintDebugHandler(const char* message, ...);

	// Is to assert debug messages
	void AssertDebugHandler(bool condition, const char* file, int line, const char* message, ...);

	// Console init
	void ConsoleInitHandler();

	void CustomTerminateHandler();

	int CustomMessageBox(const char* file, int line, const char* message);
}

// If debug diagnostics is enabled, then we will use the debug printing function
#if ENABLE_DEBUG_DIAG

#define DebugPrint(...) debug::PrintDebugHandler(__VA_ARGS__);

#define Assert(condition, ...) debug::AssertDebugHandler(condition, __FILE__, __LINE__, __VA_ARGS__);

#define Console(...) debug::ConsoleInitHandler(__VA_ARGS__);

#define CustomTerminate(...) debug::CustomTerminateHandler(__VA_ARGS__);


// Else, we will just ignore the debug printing function
#else
#define DebugPrint(...) ((void)0);

#define Assert(condition, ...) ((void)0);

#define Console(...) ((void)0);

#define CustomTerminate(...) ((void)0);

#endif
