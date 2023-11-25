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