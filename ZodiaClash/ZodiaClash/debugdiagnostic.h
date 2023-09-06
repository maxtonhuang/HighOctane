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
*//*______________________________________________________________________*/
#pragma once

#include <cstdio>
#include <cstdarg> // For va_list, va_start, va_end

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

namespace debug {
	void debugPrintHandler(const char* message, ...);
}



// If debug diagnostics is enabled, then we will use the debug printing function
#if G_ENABLE_DEBUG_DIAGNOSTICS

#define DebugPrint(...) debug::debugPrintHandler(__VA_ARGS__);

// Else, we will just ignore the debug printing function
#else
#define DebugPrint(...) ((void)0)
#endif
