// ZodiaClash.cpp : Defines the entry point for the application.
//


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

//#define MAX_LOADSTRING 100

using Vec2 = vmath::Vector2;


// Global Variables:
//HINSTANCE hInst;                                // current instance
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

bool gConsoleInitalized = false;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    nCmdShow = nCmdShow; //unused variable
    hInstance = hInstance; //unused variable
    // Enable run-time memory check for debug builds.
    #if (_DEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // To enable the console
    //Console();
    LOG_INFO("Program started");

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

    // For Wen Yuan to test his assert functions and logging
    //int x = -1;

    //YOU_ASSERT(x > 0, "x must be more than 0");
    //ASSERT(x <= 0, "x must be more than 0");
    //MY_ASSERT(x <= 0);
    //int y = -1;

    //ASSERT(y <= 0, "x must be more than 0");


    //////////////////////////////
    ////////// Run Game //////////
    //////////////////////////////

    
    EngineCore::engineCore().Run();
    return 0;
}
