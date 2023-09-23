
#include "GameStateManager.h"
#include "Battle.h"

int current = 0, previous = 0, next = 0;

FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr, fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;

void GSM_Initialize(int startingState)
{
	current = previous = next = startingState;
}

void GSM_Update()
{
	switch (next)
	{
	//case GS_MAINMENU:
	//	fpLoad = MainMenu_Load;
	//	fpInitialize = MainMenu_Initialize;
	//	fpUpdate = MainMenu_Update;
	//	fpDraw = MainMenu_Draw;
	//	fpFree = MainMenu_Free;
	//	fpUnload = MainMenu_Unload;
	//	break;
	//case GS_CREDITS:
	//	fpLoad = Credits_Load;
	//	fpInitialize = Credits_Initialize;
	//	fpUpdate = Credits_Update;
	//	fpDraw = Credits_Draw;
	//	fpFree = Credits_Free;
	//	fpUnload = Credits_Unload;
	//	break;
	//case GS_EXPLORE:
	//	fpLoad = Level_Load;
	//	fpInitialize = Level_Initialize;
	//	fpUpdate = Level_Update;
	//	fpDraw = Level_Draw;
	//	fpFree = Level_Free;
	//	fpUnload = Level_Unload;
	//	break;
	case GS_BATTLE:
		fpLoad = Battle_Load;
		fpInitialize = Battle_Initialize;
		fpUpdate = Battle_Update;
		fpDraw = Battle_Draw;
		fpFree = Battle_Free;
		fpUnload = Battle_Unload;
		break;
	case GS_RESTART:
		break;
	case GS_QUIT:
		break;
	default:
		break;
	}
}

void GSM_Exit() {

}