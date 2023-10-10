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
*	@file		DebugDiagnostic.h
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
*	@date		30 August 2023
*
* *****************************************************************************
*
*	@brief		This file contains the functions declaration for debugging
*
*   1. Include this header file in the file you want to use the functions
*
*	2.	\code{.cpp}
*			DEBUG_PRINT("Debug message: %s %d", "Value:", 42);
*		\endcode
*
*	This will print out the following message to the standard error stream:
*	"Debug message: Value: 42"
*
*	3.	\code{.cpp}
*			int x = -1;
*			ASSERT(x < 0, "x must be more than 0");
*		\endcode
*	
*		This will print out and (log to error file?) the following message to the standard error stream:
*		Assertion failed in {code.cpp} {line }: x must be more than 0
*		Assert will have a custom message box appear, user have a chance to
*		choose no to continue on or yes to quit the program and call abort().
******************************************************************************/
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
#include <cstdlib>
#include <assert.h>

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

// If debug diagnostics is enabled, then we will use the debug printing function
#if ENABLE_DEBUG_DIAG

#define DEBUG_PRINT(...) debug::PrintDebugHandler(__VA_ARGS__);

#define ASSERT(condition, ...) debug::AssertDebugHandler(condition, __FILE__, __LINE__, __VA_ARGS__);

#define Console(...) debug::ConsoleInitHandler(__VA_ARGS__);

#define CustomTerminate(...) debug::CustomTerminateHandler(__VA_ARGS__);


// Else, we will just ignore the debug printing function
#else
#define DEBUG_PRINT(...) ((void)0);

#define ASSERT(condition, ...) ((void)0);

#define Console(...) ((void)0);

#define CustomTerminate(...) ((void)0);

#endif

#ifdef CUSTOMENGINE_EXPORTS
#define CUSTOMENGINE_API __declspec(dllexport)
#else 
#define CUSTOMENGINE_API __declspec(dllimport)
#endif

namespace debug {
	
	// Is to print debug messages
	CUSTOMENGINE_API void PrintDebugHandler(const char* message, ...);

	// Is to assert debug messages
	CUSTOMENGINE_API void AssertDebugHandler(bool condition, const char* file, int line, const char* message, ...);

	// Console init
	CUSTOMENGINE_API void ConsoleInitHandler();

	CUSTOMENGINE_API void CustomTerminateHandler();

	CUSTOMENGINE_API int CustomMessageBox(const char* file, int line, const char* message);
}


