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
*	@file		Cheatcode.h
*
*	@author		Liu WanTing
*
*	@email		wanting.liu\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 November 2023
*
* *****************************************************************************
*
*	@brief		Game Cheat Code
*
*	This file contains implementations of functions to handle the cheatcode, 
*   godMode and endGameMode
*
******************************************************************************/
#pragma once
#include "CharacterStats.h"

extern bool godModeOn;
extern bool endGameOn;
void RestoreFullHealth(CharacterStats& character);
void DestroyAllHealth(CharacterStats& character);
void ToggleGodMode();
void ToggleEndGameMode();
void ActivateNextLevel();
void ActivateEndGame();