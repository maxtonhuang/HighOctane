#pragma once
#include "ImGuiLib.h"
#include "graphics.h"


class GUIManager {
public:
        GUIManager();

        ~GUIManager();

        void Init(GLFWwindow* window);
        
        void Update(GLFWwindow* window);

private:
  

};
    extern GUIManager guiManager;
