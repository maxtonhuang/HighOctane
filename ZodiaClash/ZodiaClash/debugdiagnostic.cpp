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

    // Variables
    float performanceTime{};

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
        const int bufferSize{ 1024 };
        char buffer[bufferSize];
        vsnprintf(buffer, bufferSize, message, args);

        // Clean up the variable argument list
        va_end(args);

        // Print the formatted message to the standard error stream
        fprintf(stderr, "%s\n", buffer);

        // Log it into the file
        debuglog::logger.info(buffer);
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
            if (condition) {

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

                // Create the logging file only when needed
                debuglog::Logger crashLogger("crash.log", debuglog::LOG_LEVEL::Trace, true);

                // Print an error message with just the file name and line number
                std::cerr << "Assertion failed in " << fileName << " line " << line << ": ";

                // Format and print the custom message (if provided)
                if (message) {
                    va_list args;
                    va_start(args, message);
                    vfprintf(stderr, message, args);
                    va_end(args);

                    // Log the crash into the crash file
                    crashLogger.error("Assertion failed in " + std::string(fileName) + " line " + std::to_string(line) + ": " + message);

                }

                std::cerr << std::endl;

                std::abort();
            }
        } while (false);
    }


    // Creates the console
    void consoleInitHandler() {
        // Allocate a new console for the calling process
        AllocConsole();

        // Attach the console to the current process
        AttachConsole(GetCurrentProcessId());

        // Set the title of the console
        SetConsoleTitle(L"ZodiaClash - Console");

        FILE* newStdout;
        freopen_s(&newStdout, "CONIN$", "r", stdin); // If need to read from console
        freopen_s(&newStdout, "CONOUT$", "w", stderr); // If need to write to console
        freopen_s(&newStdout, "CONOUT$", "w", stdout); // If need to write to console

        // Just for fun
        std::cout << "ZodiaClash Engine Version 0.1\n";

    }

    // Function to print out the memory usage
    void performanceDataHandler() {

        PROCESS_MEMORY_COUNTERS pmc;

        // For the CPU usage
        FILETIME idleTime, kernelTime, userTime;
        performanceTime++;

        if (performanceTime > 300) {
			performanceTime = 0;
			
            // To get the memory usage in bytes
            if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
                Assert(!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)), "Unable to get memory");
                // Working set size in bytes
                SIZE_T usedMemory = pmc.WorkingSetSize;
                std::cout << "Used memory: " << usedMemory / (1024 * 1024) << " MB" << std::endl;
                
            }

            // To get the CPU percentage usage
            if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
                ULARGE_INTEGER idle, kernel, user;
                idle.LowPart = idleTime.dwLowDateTime;
                idle.HighPart = idleTime.dwHighDateTime;
                kernel.LowPart = kernelTime.dwLowDateTime;
                kernel.HighPart = kernelTime.dwHighDateTime;
                user.LowPart = userTime.dwLowDateTime;
                user.HighPart = userTime.dwHighDateTime;

                // Calculate CPU usage percentage
                double total = kernel.QuadPart + user.QuadPart;
                double idlePercent = (static_cast<double>(idle.QuadPart) / total) * 100.0;
                double usagePercent = 100.0 - idlePercent;
                std::cout << "CPU Usage: " << usagePercent << "%" << std::endl;

                // If CPU usage is more than 50%
                Assert(usagePercent > 50.f, "CPU usage is more than 50%!");
            }  
        }
    }

    void CustomTerminateHandler() {
        // Log the unhandled exception information

        Assert(true, "Unhandled exception occurred");
        // Terminate the program
        std::abort();
    }

} // namespace debug

