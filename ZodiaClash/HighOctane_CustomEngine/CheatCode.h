#pragma once
#include "CharacterStats.h"

extern bool godModeOn;
void RestoreFullHealth(CharacterStats& character);
void ToggleGodMode();
void ActivateNextLevel();
void ActivateEndGame();