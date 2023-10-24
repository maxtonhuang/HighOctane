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
*	@file		ImGuiConsole.cpp
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
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		Console window for ImGui
*
*	This file contains all the definition of the functions for the console window
*   It also contains the definition of the stream buffer that will be used to 
*   redirect std::cout to ImGui
*
******************************************************************************/
#include "ImGuiConsole.h"
#include "GUIManager.h"
#include "MultiThreading.h"
#include "debugdiagnostic.h"

/*!
* \brief Init the console window
*
* This function is responsible for initialising the console window
* It will be called in ImGuiManager.cpp
*
*/
void InitConsole() {

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

}

/*!
* \brief Update the console window
*
* This function is responsible for updating the console window
* It will be called in ImGuiManager.cpp
*
*/
void UpdateConsole() { 
    
    ImGui::Begin("Testing script thing");

    ImGui::Text("Hello world");
    if (ImGui::Button("Test add script")) {
        std::cout << "Script added" << std::endl;
    }
    ImGui::End();

    static char filterBuffer[256] = "";
    static char fileNameBuffer[31] = "";
    static bool autoScroll = true;
    // Change the colour for my console window
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Change to your desired color

    if (ImGui::Begin("Console")) {

        // Push the width of the input text box
        ImGui::PushItemWidth(200);

        /********FILE INPUT TEXT*********/
        ImGui::InputText("File Name", fileNameBuffer, IM_ARRAYSIZE(fileNameBuffer), ImGuiInputTextFlags_CharsNoBlank);
        ImGui::SameLine();
        ImGui::TextDisabled("(?)"); // Label text
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Enter name of file. Eg. 'Good' will create a file called 'Good.log' ");
        }
        /********FILE INPUT TEXT*********/

        // Export to file button
        ImGui::SameLine();
        if (ImGui::Button("Export to File")) {
            // Call a function to export the console content to a file
            ExportConsoleToFile(fileNameBuffer);
            DeleteLineFromFile(fileNameBuffer);
        }
        //ImGui::Separator();
        ImGui::SameLine(0.0f, 50.0f);
        /********FILTER INPUT TEXT*********/
        ImGui::InputText("Filter", filterBuffer, IM_ARRAYSIZE(filterBuffer));
        ImGui::SameLine();
        ImGui::TextDisabled("(?)"); // Label text
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Filters console. Is case-insensitive");
        }
        /********FILTER INPUT TEXT*********/

        // Pop the input text box width
        ImGui::PopItemWidth();
        //ImGui::Separator();
        ImGui::SameLine(0.0f, 50.0f);
        // Checkbox for the auto scroll
        ImGui::Checkbox("Auto-Scroll", &autoScroll);
        //ImGui::Separator();
        ImGui::SameLine(0.0f, 50.0f);
        // For clearing buffer
        if (ImGui::Button("Clear")) {
            imguiOutputBuffer.ClearBuffer();
        }
        ImGui::Separator();

        // Create a scrolling region for the content
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        // Display the captured output in the text box
        const std::string& logBuffer = imguiOutputBuffer.GetBuffer();
        size_t pos = 0;
        while (pos < logBuffer.size()) {
            size_t newlinePos = logBuffer.find('\n', pos);
            if (newlinePos != std::string::npos) {
                std::string logEntry = logBuffer.substr(pos, newlinePos - pos);

                // Check if the log entry contains the filter string (case-insensitive)
                std::string logEntryLowerCase = logEntry;
                std::string filterLowerCase = filterBuffer;

                // Convert both strings to lower case
                std::transform(logEntryLowerCase.begin(), logEntryLowerCase.end(), logEntryLowerCase.begin(),
                    [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });

                std::transform(filterLowerCase.begin(), filterLowerCase.end(), filterLowerCase.begin(),
                    [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });


                /********CUSTOM COLLOUR OUTPUT*********/
                ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default to white text color

                if (logEntryLowerCase.find("[trace]") != std::string::npos) {
                    textColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);  // Grey for trace messages
                }
                else if (logEntryLowerCase.find("[debug]") != std::string::npos) {
                    textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow for debug messages
                }
                else if (logEntryLowerCase.find("[info]") != std::string::npos) {
                    textColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);  // Blue for info messages
                }
                else if (logEntryLowerCase.find("[warning]") != std::string::npos) {
                    textColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);  // Orange for warning messages
                }
                else if (logEntryLowerCase.find("[error]") != std::string::npos) {
                    textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red for error messages
                }
                else if (logEntryLowerCase.find("[fatal]") != std::string::npos) {
                    textColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);  // Bright red for fatal messages
                }
                /********CUSTOM COLLOUR OUTPUT*********/

                // Set the text color
                if (logEntryLowerCase.find(filterLowerCase) != std::string::npos) {
                    //ImGui::TextUnformatted(logEntry.c_str());
                    ImGui::TextColored(textColor, logEntry.c_str());
                }
                pos = newlinePos + 1;
            }
            else {
                break;
            }
        }

        if (autoScroll) {
            ImGui::SetScrollHereY(1.0f);
        }

        // End the scrolling region
        ImGui::EndChild();
    }
    // End the console window
    ImGui::End();

    // Stops changing the colour
    ImGui::PopStyleColor();
}


/*!
* \brief Stream buffer for ImGui
*
* This function is responsible for creating a stream buffer for ImGui
* It will be used to redirect std::cout to ImGui
*
* \param c The character to be appended to the buffer
*/
int ImGuiOutputBuffer::overflow(int c) {
    if (c != EOF) {
        // Append the character to a buffer
        buffer += static_cast<char>(c);
    }
    return c;
}

/*!
* \brief Get the buffer for ImGui
*
* This function is responsible for getting the buffer for ImGui
* 
*/
const std::string& ImGuiOutputBuffer::GetBuffer() const {
    return buffer;
}

/*!
* \brief Clear the buffer for ImGui
*
* This function is responsible for clearing the buffer for ImGui
* 
*/
void ImGuiOutputBuffer::ClearBuffer() {
    buffer.clear();
}

// Define the instance
ImGuiOutputBuffer imguiOutputBuffer;

// Redirect std::cout to use the custom stream buffer
std::ostream imguiCout(&imguiOutputBuffer);
std::streambuf* coutBuf = std::cout.rdbuf(imguiCout.rdbuf());

/*!
* \brief Export the console content to a file
*
* This function is responsible for exporting the console content to a file
*
* \param fileName The name of the file to export to
*/
void ExportConsoleToFile(const char* fileName) {
    std::string fullFileName;

    // If the file name is empty, use the default file name
    fileName&& fileName[0] != '\0' ? fullFileName = std::string(fileName) + ".log" : fullFileName = "Console.log";

    std::ofstream outputFile(fullFileName);

    ASSERT(!outputFile.is_open(), "Unable to open file");

    // Get the console content and write it to the file
    const std::string& logBuffer = imguiOutputBuffer.GetBuffer();
    outputFile << logBuffer;

    // Close the file
    outputFile.close();

    // Clear the console after exporting, optional
    imguiOutputBuffer.ClearBuffer();

    // Display success message after exporting, optional
    std::cout << "Console content exported to '" << fullFileName << "'." << std::endl;
}

/*!
* \brief Delete the line from the file
*
* This function is responsible for deleting the line from the file
*
* \param fileName The name of the file to delete from
*/
void DeleteLineFromFile(const char* fileName) {
    std::string fullFileName = std::string(fileName) + ".log";
    std::ifstream inputFile(fullFileName);
    ASSERT(!inputFile.is_open(), "Unable to open file to read");

    std::string line;
    std::string fileContents;

    // Read the entire file into a string, excluding the line to delete
    while (std::getline(inputFile, line)) {
        if (line.find("Console content exported to '") == std::string::npos) {
            fileContents += line + "\n";
        }
    }

    inputFile.close();

    // Open the file to write
    std::ofstream outputFile(fullFileName, std::ios::trunc);
    ASSERT(!outputFile.is_open(), "Unable to open file to write");

    // Write the string to the file
    outputFile << fileContents;
    outputFile.close();
}