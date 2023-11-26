#include "CheatCode.h"
#include "EntityFactory.h"

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
	static Entity cheatlabel{};
	if (godModeOn) {
		if (!ECS::ecs().EntityExists(cheatlabel)) {
			cheatlabel = EntityFactory::entityFactory().ClonePrefab("cheattext.prefab");
		}
	}
	else {
		if (ECS::ecs().EntityExists(cheatlabel)) {
			EntityFactory::entityFactory().DeleteCloneModel(cheatlabel);
			cheatlabel = 0;
		}
	}
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