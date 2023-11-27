#include "CheatCode.h"
#include "EntityFactory.h"

bool godModeOn{ false };
bool endGameOn{ false };

/**
 * RestoreFullHealth - Restores the full health of a given character.
 *
 * @param character - A reference to the CharacterStats object whose health is to be restored.
 */
void RestoreFullHealth(CharacterStats& character) {
	if (godModeOn) {
		if (character.tag == CharacterType::PLAYER) {
			character.stats.health = character.stats.maxHealth;
		}
	}
}

/**
 * DestroyAllHealth - Reduces the health of a given character to zero.
 *
 * @param character - A reference to the CharacterStats object whose health is to be destroyed.
 */
void DestroyAllHealth(CharacterStats& character) {
	if (endGameOn) {
		if (character.tag == CharacterType::ENEMY) {
			character.stats.health = 0.f;
		}
	}
}

/**
 * ToggleGodMode - Toggles the god mode on or off.
 *
 * When god mode is on, the character becomes invincible.
 */
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

/**
 * ToggleEndGameMode - Toggles the end game mode on or off.
 *
 * When end game mode is on, the game will proceed to its ending sequence.
 */
void ToggleEndGameMode() {
	endGameOn = !endGameOn;
}

/**
 * ActivateNextLevel - Triggers the transition to the next level in the game.
 */
void ActivateNextLevel() {
	GameStateMgr::GetInstance()->NextLevel();
}

/**
 * ActivateEndGame - Initiates the end game sequence.
 */
void ActivateEndGame() {
	GameStateMgr::GetInstance()->GoToEndLevel();
}