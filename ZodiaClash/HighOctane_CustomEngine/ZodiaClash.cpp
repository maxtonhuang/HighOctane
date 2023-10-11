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
*	@file		ZodiaClash.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
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
*	@brief		Main Entry Point of Program
*
*	This file contains the main entry point of the program created using the
*   standard Visual Studio Desktop Application Wizard. A lot of unnecessary
*   functions that are not applicable for use in our game were removed from the
*   standard template.
*
******************************************************************************/

// ZodiaClash.cpp : Defines the entry point for the application.

////////// MAIN ///////////

#include "Framework.h"
#include "ZodiaClash.h"
#include "Graphics.h"
#include "DebugLog.h"
#include "DebugDiagnostic.h"
#include "Input.h"
#include "EngineCore.h" 
#include "Message.h"
#include "Texture.h"
#include "ECS.h"
#include "Components.h"
#include <filesystem>
#include <thread>
#include "AudioManager.h"
#include "Serialization.h"
#include "Scripting.h"
#include <mono/metadata/assembly.h>

//#define MAX_LOADSTRING 100

using Vec2 = vmath::Vector2;


// Global Variables:
// HINSTANCE hInst;                                // current instance
// WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
// WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
// ATOM                MyRegisterClass(HINSTANCE hInstance);
// BOOL                InitInstance(HINSTANCE, int);
// LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
// INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

bool gConsoleInitalized = false;

// The following will force the OS to choose Dedicated GPU. //////////

/******************************************************************************
*
*	@brief Force Dedicated GPU
*
*	The following 2 snippets of code will force the OS to choose Dedicated GPU.
*
******************************************************************************/

// NVIDIA Optimus dedicated GPU hint.
extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

// AMD PowerXpress high-performance request.
extern "C" {
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{   
    //InitMono();
    try {
        LoadCSharpScript();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    LoadConfig();
    nCmdShow = nCmdShow; //unused variable
    hInstance = hInstance; //unused variable
    // Enable run-time memory check for debug builds.
    #if (_DEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // To enable the console
    Console();
    LOG_INFO("Program started");

    // Load Configuration File

    /*--------------FOR DEBUGGING PLEASE DO NOT TOUCH FIRST THANK YOU VERY MUCH--------------------*/
    LOG_SET_LEVEL(debuglog::LOG_LEVEL::Trace);

    LOG_TRACE("This is a test trace message");
    LOG_DEBUG("This is a test debug message");
    LOG_INFO("This is a test info message");
    LOG_WARNING("This is a test warning message");
    LOG_ERROR("This is a test error message");
    LOG_FATAL("This is a test fatal message");
    /*---------------------------------------------------------------------------------------------*/
    // TODO: Place code here.
    //graphics.Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);
    audio.Initialize();
    audio.AddSound("../Assets/Sound/ping.wav");
    audio.AddSound("../Assets/Sound/bonk.wav");
    LOG_INFO("Graphics started");



    //////////////////////////////
    ////////// Run Game //////////
    //////////////////////////////
        
    EngineCore::engineCore().Run();

    //////////////////////////////
    ////////// End Game //////////
    //////////////////////////////



    return 0;
}


// // TESTING FOR CONSOLE MODE FOR SCRIPTING - WEN YUAN
//int main()
//{
//    LoadCSharpScript();
//    LoadConfig();
//
//    // Enable run-time memory check for debug builds.
//    #if (_DEBUG)
//        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//    #endif
//        
//    EngineCore::engineCore().Run();
//
//    return 0;
//
//}


