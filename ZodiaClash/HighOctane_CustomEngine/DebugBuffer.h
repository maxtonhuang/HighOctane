#pragma once
#include <iostream>

// ALL THESE IS FOR DEBUGGING PLEASE DO NOT TOUCH AT ALL
// This is the stream buffer that will be used to redirect std::cout to ImGui
class ImGuiOutputBuffer : public std::streambuf {
public:
	int overflow(int c);
	const std::string& GetBuffer() const;
	void ClearBuffer();
	std::string buffer;
private:

};

extern ImGuiOutputBuffer imguiOutputBuffer; // Declare as extern for use in other files