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
*	This file contains the declarations of the different parts of the Game
*	State Manager.
*
******************************************************************************/

#pragma once

typedef void(*FP)(void);

extern int gsmCurrent, gsmPrevious, gsmNext;

extern FP fpLoad, fpInitialize, fpUpdate, fpDraw, fpFree, fpUnload;

void GSM_Initialize(int startingState);

void GSM_Update();

void GSM_Exit();

enum GS_STATES
{
	GS_MAINMENU = 0,
	GS_CREDITS,
	GS_EXPLORE,
	GS_BATTLE,

	GS_QUIT,
	GS_RESTART
};
