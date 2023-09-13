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

    /*!
     * \brief Prints a debug message to the standard error stream.
     *
     * This function is responsible for printing debug messages to the standard error stream.
     * It allows you to provide a formatted message along with variable arguments for flexible
     * debugging output.
     *
     * \param message The format string for the debug message.
     * \param ... Additional arguments for formatting the debug message.
     * 
     */
	void printDebugHandler(const char* message, ...) {

        // Create a variable argument list
        va_list args;

        // Initialize the variable argument list
        va_start(args, message);

        // Use vsnprintf to format the message with variable arguments into a buffer
        const int bufferSize = 1024;
        char buffer[bufferSize];
        vsnprintf(buffer, bufferSize, message, args);

        // Clean up the variable argument list
        va_end(args);

        // Print the formatted message to the standard error stream
        fprintf(stderr, "%s\n", buffer);
	}


    /*!
     * \brief Handles assertion failures and provides detailed error messages.
     * This function is responsible for handling assertion failures. It checks the specified
     * condition and, if the condition is false, prints an error message to the standard error
     * stream. The error message includes the file name, line number, and an optional custom
     * message.
     *
     * \param condition A boolean expression representing the assertion condition.
     * \param file The source file path where the assertion occurred.
     * \param line The line number in the source file where the assertion occurred.
     * \param message A custom error message (optional) to be included in the output.
     * \param ... Additional arguments for formatting the custom message (if provided).
     *
     */
    void assertDebugHandler(bool condition, const char* file, int line, const char* message, ...) {
        do {

            // If the condition is true, then we don't need to do anything
            if (!condition) {

                // Extract the file name from the file path
                const char* fileName = std::strrchr(file, PATH_SEPARATOR[0]);

                // If no '/' is found, use the entire file path
                if (fileName == nullptr) {
                    fileName = file;
                }

                // Move past the '/' character
                else {
                    fileName++;
                }

                // Print an error message with just the file name and line number
                std::cerr << "Assertion failed in " << fileName << " line " << line << ": ";

                // Format and print the custom message (if provided)
                if (message) {
                    va_list args;
                    va_start(args, message);
                    vfprintf(stderr, message, args);
                    va_end(args);
                }

                std::cerr << std::endl;

                std::abort();
            }
        } while (false);
    }

} // namespace debug

