#include "CheatCode.h"

bool godModeOn{ false };
bool endGameOn{ false };

void RestoreFullHealth(CharacterStats& character) {
	if (godModeOn) {
		if (character.tag == CharacterType::PLAYER) {
			character.stats.health = character.stats.maxHealth;
		}
	}
}

void DestroyAllHealth(CharacterStats& character) {
	if (endGameOn) {
		if (character.tag == CharacterType::ENEMY) {
			character.stats.health = 0.f;
		}
	}
}

void ToggleGodMode() {
	godModeOn = !godModeOn; // reset the state back to non god mode
}

void ToggleEndGameMode() {
	endGameOn = !endGameOn;
}

void ActivateNextLevel() {
	GameStateMgr::GetInstance()->NextLevel();
}

void ActivateEndGame() {
	GameStateMgr::GetInstance()->GoToEndLevel();
}