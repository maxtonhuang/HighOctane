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
#include "AudioManager.h"

//#define MAX_LOADSTRING 100

using Vec2 = vmath::Vector2;

extern Mail mail;

using namespace Architecture;

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
    // Enable run-time memory check for debug builds.
    #if defined(DEBUG) | defined(_DEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    
    

    // To enable the console
    Console();
    debuglog::logger.info("Program started");

    

    /*--------------FOR DEBUGGING PLEASE DO NOT TOUCH FIRST THANK YOU VERY MUCH--------------------*/
    debuglog::logger.SetLevel(debuglog::LOG_LEVEL::Trace);

    debuglog::logger.trace("This is a test trace message");
    debuglog::logger.debug("This is a test debug message");
    debuglog::logger.info("This is a test info message");
    debuglog::logger.warning("This is a test warning message");
    debuglog::logger.error("This is a test error message");
    debuglog::logger.fatal("This is a test fatal message");
    /*---------------------------------------------------------------------------------------------*/

    // TODO: Place code here.
    graphics.Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);
    audio.Initialize();
    audio.AddSound("../Assets/Sound/ping.wav");
    debuglog::logger.info("Graphics started");

    //DEBUG GRAPHIC MODELS FOR COLLISION TESTING
    test_circle1.AttachTexture("circle.png");
    test_circle2.AttachTexture("circle.png");
    test_circle3.AttachTexture("circle.png");

    test_circle1.SetDim(100, 100);
    test_circle1.SetPos(100, 100);

    test_circle2.SetDim(100, 100);
    test_circle2.SetPos(200, 200);

    test_circle3.SetDim(100, 100);
    test_circle3.SetPos(300, 300);

    test_rect1.SetDim(100, 100);
    test_rect1.SetPos(-100, -100);

    test_rect2.SetDim(200, 100);
    test_rect2.SetPos(-200, -200);

    test_rect3.SetDim(100, 200,50);
    test_rect3.SetPos(-300, -300);

    // For Wen Yuan to test his assert functions and logging
    //int x = -1;

    //Assert(x <= 0, "x must be more than 0");

    // Max

    EngineCore* engine = new EngineCore();
        
    engine->Run();



    /*MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }*/

    //// Main message loop:
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}

    /*--------------FOR DEBUGGING PLEASE DO NOT TOUCH FIRST THANK YOU VERY MUCH--------------------*/


    //_CrtDumpMemoryLeaks();

    /*return (int) msg.wParam;*/
    
    delete engine;
    
    return 0;
}

