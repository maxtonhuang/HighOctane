/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		debugdiagnostic.cpp
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		30 August 2023
@brief		This file contains the functions definition for debugging
*//*______________________________________________________________________*/

#include "DebugDiagnostic.h"

namespace debug {

	// This function is used to print debug messages
	void debugPrintHandler(const char* message, ...) {
        // Create a variable argument list
        va_list args;
        va_start(args, message);

        // Use vsnprintf to format the message with variable arguments
        // and store it in a buffer
        const int bufferSize = 1024;
        char buffer[bufferSize];
        vsnprintf(buffer, bufferSize, message, args);

        // Clean up the variable argument list
        va_end(args);

        // Print the formatted message to the standard error stream
        fprintf(stderr, "%s\n", buffer);

        // You can also add code here to log the message to a file or perform
        // other actions as needed for debugging and diagnostics.
	}
}
