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
	void PrintDebugHandler(const char* message, ...) {

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
        //debuglog::logger.info(buffer);
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
    void AssertDebugHandler(bool condition, const char* file, int line, const char* message, ...) {
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

                // Display a message box to the user
                int testing = CustomMessageBox(fileName, line, message);
            }
        } while (false);
    }

    // For the message in a box to use in assert
    int CustomMessageBox(const char* file, int line, const char* message) {

        // Convert the const char* to LPCWSTR
        std::wstring wideFile = std::wstring(file, file + strlen(file));
        std::wstring wideMessage = std::wstring(message, message + strlen(message));

        LPCWSTR wideFilePtr = wideFile.c_str();
        LPCWSTR wideMessagePtr = wideMessage.c_str();

        // Creating the custom message
        std::wstring customMsg = L"In file: " + std::wstring(wideFilePtr) + L" line: " + std::to_wstring(line)\
            + L"\n\nAssert triggered: " + std::wstring(wideMessagePtr)\
            + L"\n\nYes to quit\n\nNo to continue"\
            + L"\nWARNING: PROGRAM MAY NOT WORK PROPERLY";
        std::wstring customTitle = +L"Quit program?";

        // Display a message box to the user
        int msgboxID = MessageBox(
            NULL,
            customMsg.c_str(),
            customTitle.c_str(),
            MB_ICONERROR | MB_YESNO | MB_DEFBUTTON1 | MB_DEFAULT_DESKTOP_ONLY
        );
        // Create the logging file only when needed
        debuglog::Logger crashLogger("crash.log", debuglog::LOG_LEVEL::Trace);
        switch (msgboxID)
        {
        case IDYES:
            // Log the crash into the crash file
            crashLogger.error("Assertion failed in " + std::string(file) + " line " + std::to_string(line)\
            + ". User chose to terminate");
            ExitProcess(0);
            break;
        case IDNO:
            crashLogger.error("Assertion failed in " + std::string(file) + " line " + std::to_string(line)\
            + ". User chose to continue");
            // Continue on with the code
            break;
        }

        return msgboxID;
    }

    // Creates the console
    void ConsoleInitHandler() {
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

    void CustomTerminateHandler() {
        // Log the unhandled exception information

        Assert(true, "Unhandled exception occurred");
        // Terminate the program
        std::abort();
    }

} // namespace debug

