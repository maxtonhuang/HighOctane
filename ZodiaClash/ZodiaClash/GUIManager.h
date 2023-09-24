#pragma once
#include "ImGuiLib.h"
#include "graphics.h"

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

class GUIManager {
public:
        GUIManager();

        ~GUIManager();

        void Init(GLFWwindow* window);
        
        void Update(GLFWwindow* window);

private:
  

};
    extern GUIManager guiManager;