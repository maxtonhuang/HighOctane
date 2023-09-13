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

//#define MAX_LOADSTRING 100

// For debugging
debuglog::Logger logger("test.log", debuglog::LOG_LEVEL::Trace, ENABLE_DEBUG_DIAG);

using Vec2 = vmath::Vector2;

extern Mail mail;

using namespace Architecture;

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
    debuglog::zcSysInit(hInstance, nCmdShow, 700, 700, ENABLE_DEBUG_DIAG, 60,true);
    logger.info("Program started");

    

    // TODO: Place code here.
    graphics.Initialize(1000, 1000);
    logger.info("Graphics started");



    EngineCore* engine = new EngineCore();
        
    engine->Run();



    /*--------------FOR DEBUGGING PLEASE DO NOT TOUCH FIRST THANK YOU VERY MUCH--------------------*/
    logger.setLevel(debuglog::LOG_LEVEL::Trace);

    logger.trace("This is a test trace message");
    logger.debug("This is a test debug message");
    logger.info("This is a test info message");
    logger.warning("This is a test warning message");
    logger.error("This is a test error message");
    logger.fatal("This is a test fatal message");
    /*---------------------------------------------------------------------------------------------*/

    //_CrtDumpMemoryLeaks();

    /*return (int) msg.wParam;*/
    
    delete engine;
    
    return 0;
}

