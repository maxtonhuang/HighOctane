#include "DebugBuffer.h"
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