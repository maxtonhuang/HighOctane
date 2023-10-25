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
*	@file		GameStateManager.h
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
*	@brief		Game State Manager
*
*	This file contains the main update function for the Game State Manager. It
*	loads the corresponding function pointers when the state of the game is
*	changed. 
*
******************************************************************************/

#include "GameStateManager.h"
#include "Battle.h"
#include "CharacterStats.h"

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