#include "CheatCode.h"

bool godModeOn{ false };

void RestoreFullHealth(CharacterStats& character) {
	if (godModeOn) {
		if (character.tag == CharacterType::PLAYER) {
			character.stats.health = character.stats.maxHealth;
		}
	}
}

void ToggleGodMode() {
	godModeOn = !godModeOn; // reset the state back to non god mode
}

void ActivateNextLevel() {
	GameStateMgr::GetInstance()->NextLevel();
}

void ActivateEndGame() {
	GameStateMgr::GetInstance()->GoToEndLevel();
}