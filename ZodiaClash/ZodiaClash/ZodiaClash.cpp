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

//#define MAX_LOADSTRING 100

// For debugging
debuglog::Logger logger("test.log", debuglog::LOG_LEVEL::Trace, ENABLE_DEBUG_DIAG);



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
    debuglog::zcSysInit(hInstance, nCmdShow, 700, 700, ENABLE_DEBUG_DIAG, 60,true);
    logger.info("Program started");

    //Mail mail;
    

    // TODO: Place code here.
    graphics.Initialize(1000, 1000);
    logger.info("Graphics started");


    // Max


        EngineCore* engine = new EngineCore();
        
        
        
        engine->GameLoop();








    // 

    /*
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ZODIACLASH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    std::cout << "Current file: " << __FILE__ << std::endl;
    std::cout << "Current line: " << __LINE__ << std::endl;
    std::cout << "Current function: " << __FUNCTION__ << std::endl;  

    // Game loop here please change it in the future
    int test = 0;
    while (!GraphicsManager::WindowClosed())
    {
        glfwPollEvents();
        GraphicsManager::Draw();
        test++;
	}

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZODIACLASH));
    
    MSG msg;
    //// Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_ZODIACLASH, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);

    //// Perform application initialization:
    //if (!InitInstance (hInstance, nCmdShow))
    //{
    //    return FALSE;
    //}

    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZODIACLASH));

    //MSG msg;

    // ////////// INTERCEPT MESSAGE HERE.
    // 
    // 
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return (int) msg.wParam;
    */



    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

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

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//    WNDCLASSEXW wcex;
//
//    wcex.cbSize = sizeof(WNDCLASSEX);
//
//    wcex.style          = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc    = WndProc;
//    wcex.cbClsExtra     = 0;
//    wcex.cbWndExtra     = 0;
//    wcex.hInstance      = hInstance;
//    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZODIACLASH));
//    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
//    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
//    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ZODIACLASH);
//    wcex.lpszClassName  = szWindowClass;
//    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//    return RegisterClassExW(&wcex);
//}
//
////
////   FUNCTION: InitInstance(HINSTANCE, int)
////
////   PURPOSE: Saves instance handle and creates main window
////
////   COMMENTS:
////
////        In this function, we save the instance handle in a global variable and
////        create and display the main program window.
////
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   hInst = hInstance; // Store instance handle in our global variable
//
//   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}
//
////
////  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
////
////  PURPOSE: Processes messages for the main window.
////
////  WM_COMMAND  - process the application menu
////  WM_PAINT    - Paint the main window
////  WM_DESTROY  - post a quit message and return
////
////
// 
// 



// Windows event handler
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_LBUTTONDOWN:
        // Handle other Windows events...
        extern Mail mail;
        mail.ConvertWindowsEventToPostcard(uMsg);
        return 0;
        // Handle other Windows events...
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_COMMAND:
//        {
//            int wmId = LOWORD(wParam);
//            // Parse the menu selections:
//            switch (wmId)
//            {
//            case IDM_ABOUT:
//                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//                break;
//            case IDM_EXIT:
//                DestroyWindow(hWnd);
//                break;
//            default:
//                return DefWindowProc(hWnd, message, wParam, lParam);
//            }
//        }
//        break;
//    case WM_PAINT:
//        {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
//            // TODO: Add any drawing code that uses hdc here...
//            EndPaint(hWnd, &ps);
//        }
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
//
//// Message handler for about box.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//
