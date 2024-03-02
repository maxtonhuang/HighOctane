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
*	@file		Events.cpp
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu
* 
* *	@co-author	Koh Wen Yuan (Did the events for Play/Pause and Game Help
*
*	@email		k.wenyuan\@digipen.edu 
* 
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		19 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains class for event manager of the game.
*	Events are primarily used by the buttons
*
******************************************************************************/

#include <sstream>
#include "Events.h"
#include "AssetManager.h"
#include "enginecore.h"
#include "Global.h"
#include "CharacterStats.h"
#include "Layering.h"
#include "Transition.h"
#include "UIComponents.h"
#include "ECS.h"

EventManager events;

/*!
 * \brief Exits the game menu, voids input
 *
 * std::string input : The input string.
 */
void ExitGame(std::string input) {
	(void)input;
	static Entity exitconfirmationmenu{};

	if (GetCurrentSystemMode() == SystemMode::EXITCONFIRM) {
		SetCurrentSystemMode(GetPreviousSystemMode());

		if (exitconfirmationmenu != 0) {
			ECS::ecs().DestroyEntity(exitconfirmationmenu);
			exitconfirmationmenu = 0;
		}
	}
	else {
		SetCurrentSystemMode(SystemMode::EXITCONFIRM);
		if (exitconfirmationmenu == 0) {
			exitconfirmationmenu = EntityFactory::entityFactory().ClonePrefab("exitconfirmationmenu.prefab");
		}
	}

	//EntityFactory::entityFactory().DeleteCloneModel(exitconfirmationmenu);

	//EngineCore::engineCore().setGameActive(false);
}

/*!
 * \brief Exits the game, voids input
 *
 * std::string input : The input string.
 */
void ConfirmExit(std::string input) {
	(void)input;
	EngineCore::engineCore().setGameActive(false);
}

/*!
 * \brief Changes the scene to the input scene name
 *
 * std::string input : The input string.
 */
void ChangeScene(std::string input) {
	if (sceneName == input) {
		if (GetCurrentSystemMode() == SystemMode::PAUSE) {
			events.Call("Toggle Pause", "");
		}
		return;
	}

	newScene = true;
	button_clicked = true;
	newSceneName = input;
	destroyAll = true;
	loadingBuffer = true;
	// Change the current system mode to run, probably shouldn't be here
	//currentSystemMode = SystemMode::RUN; // change to run mode

	/*RebuildLayeringAfterDeserialization();
	ExtractSkipLockAfterDeserialization();*/
	//playButton = true;
}
/*!
 * \brief Plays the input audio name to SFX group
 *
 * std::string input : The input string.
 */
void PlayAudio(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(),"SFX");
}

/*!
 * \brief Plays the input audio name to BGM group
 *
 * std::string input : The input string.
 */
void PlayMusic(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(), "BGM");
}

/*!
 * \brief Stops BGM and plays the input audio name as BGM instead
 *
 * std::string input : The input string.
 */
void RestartMusic(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	assetmanager.audio.StopGroup("BGM");
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(), "BGM");
}

/*!
 * \brief Toggles pause or resume for the input audio group
 *
 * std::string input : The input string.
 */
void PauseResumeGroup(std::string input) {
	if (assetmanager.audio.IsGroupPaused(input.c_str())) {
		assetmanager.audio.ResumeGroup(input.c_str());
	}
	else {
		assetmanager.audio.PauseGroup(input.c_str());
	}
}

/*!
 * \brief Stops all sounds in the input audio group
 *
 * std::string input : The input string.
 */void StopGroup(std::string input) {
	assetmanager.audio.StopGroup(input.c_str());
}

/*!
 * \brief Selection of the sill
 *
 * std::string input : The input string.
 */
void SelectSkill(std::string input) {
	static auto& buttonArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Button>() };
	BattleSystem* bs = events.GetBattleSystem();

	if (bs->battleState != PLAYERTURN) {
		return;
	}

	if (bs->activeCharacter->tag != CharacterType::PLAYER || bs->activeCharacter->action.entityState != WAITING) {
		return;
	}

	std::stringstream ss{ input };
	int skillnum;
	ss >> skillnum;

	// ensure the skill number is valid, incase user later input skill 10 or smth
	if (skillnum < 1 || skillnum > bs->activeCharacter->action.skills.size()) {
		//can add error msg if we want 
		return;
	}

	// check if the character has enough Chi to perform the skill
	//if (bs->chi >= bs->activeCharacter->action.skills[skillnum - 1].chiCost) {
	//	auto targets = bs->GetEnemies();
	//	bs->activeCharacter->action.targetSelect.selectedTarget = targets[0];
	//	bs->activeCharacter->action.entityState = ATTACKING;
	//}
	//else {
	//	// handle not enough Chi, ZR part?
	//}

	for (Entity& s : bs->skillButtons) {
		buttonArray.GetData(s).hoveredColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,1.f };
		buttonArray.GetData(s).defaultColor.buttonColor = glm::vec4{ 1.f,1.f,1.f,1.f };
	}
	buttonArray.GetData(bs->skillButtons[skillnum - 1]).hoveredColor.buttonColor = glm::vec4{ 0.f,1.f,0.f,1.f };
	buttonArray.GetData(bs->skillButtons[skillnum - 1]).defaultColor.buttonColor = glm::vec4{ 0.f,1.f,0.f,1.f };

	bs->activeCharacter->action.selectedSkill = bs->activeCharacter->action.skills[skillnum - 1];
	bs->CreateTargets();
	//auto targets = bs->GetEnemies();
	//bs->activeCharacter->action.targetSelect.selectedTarget = targets[0];
	//bs->activeCharacter->action.entityState = ATTACKING;
}

/*!
 * \brief Selection of the enemy
 *
 * std::string input : The input string.
 */
void SelectEnemy(std::string input) {
	BattleSystem* bs = events.GetBattleSystem();
	std::stringstream ss{ input };
	int enemynum;
	ss >> enemynum;

	auto targets = bs->GetEnemies();
	if (bs->activeCharacter->action.selectedSkill.attacktype == AttackType::ALLY) {
		targets = bs->GetPlayers();
	}
	if (bs->activeCharacter->debuffs.tauntStack > 0 && bs->activeCharacter->action.selectedSkill.attacktype != AttackType::ALLY) {
		for (CharacterStats* target : targets) {
			if (target->entity == bs->activeCharacter->debuffs.tauntTarget) {
				bs->activeCharacter->action.targetSelect.selectedTarget = target;
				break;
			}
		}
	}
	else {
		bs->activeCharacter->action.targetSelect.selectedTarget = targets[enemynum];
	}
	
	bs->activeCharacter->action.entityState = ATTACKING;
	bs->DestroyTargets();
}

/*!
 * \brief Toggling of the pause state of the game.
 *
 * std::string input : The input string.
 */
void TogglePause(std::string input) {

	if (GetCurrentSystemMode() == SystemMode::GAMEHELP || GetCurrentSystemMode() == SystemMode::EDIT) {

		return;
	}

	(void)input;
	static Entity pausemenu{};

	/*-----Prevent Softlocking-----*/
	if (GetPreviousSystemMode() == SystemMode::GAMEHELP && GetCurrentSystemMode() == SystemMode::PAUSE) {
		SetCurrentSystemMode(SystemMode::RUN);
		if (pausemenu != 0) {
			EntityFactory::entityFactory().DeleteCloneModel(pausemenu);
			pausemenu = 0;
		}
	}
	/*-----Prevent Softlocking-----*/

	else if (!(GetPreviousSystemMode() == SystemMode::GAMEHELP) && GetCurrentSystemMode() == SystemMode::PAUSE) {
		SetCurrentSystemMode(GetPreviousSystemMode());
		if (pausemenu != 0) {
			EntityFactory::entityFactory().DeleteCloneModel(pausemenu);
			pausemenu = 0;
		}
	}
	else {
		SetCurrentSystemMode(SystemMode::PAUSE);
		if (pausemenu == 0) {
			pausemenu = EntityFactory::entityFactory().ClonePrefab("pausemenu.prefab");
		}
	}
}

/*!
 * \brief Toggling of the help state of the game.
 *
 * std::string input : The input string.
 */
void ToggleHelp(std::string input) {
	(void)input;


	static Entity gamehelpmenu{};
	if (GetCurrentSystemMode() == SystemMode::GAMEHELP) {
		SetCurrentSystemMode(GetPreviousSystemMode());

		if (gamehelpmenu != 0) {
			ECS::ecs().DestroyEntity(gamehelpmenu);
			gamehelpmenu = 0;
		}
	}
	else {
		SetCurrentSystemMode(SystemMode::GAMEHELP);
		if (gamehelpmenu == 0) {
			gamehelpmenu = EntityFactory::entityFactory().ClonePrefab("gamehelpmockup.prefab");
		}
	}
}

/*!
 * \brief Transitions from current scene to target scene.
 *
 * std::string input : The input string.
 */
void TransitionScene(std::string input) {
	transitionActive = true;
	transitionNextScene = input;
	transitionType = true;
}

/*!
 * \brief Test function.
 *
 * std::string input : The input string.
 */
void TestFunction(std::string input) {
	std::cout << input << "\n";
}

/*!
 * \brief Explicit event trigger to start dialogue if a DialogueHUD tagged component is on scene.
 *
 * std::string input : The input string.
 */
void StartDialogue(std::string input) {
	(void)input;

	UIDialogueSystem* ds = events.GetDialogueSystem();
	auto& dialogueHudArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<DialogueHUD>() };

	for (Entity const& entity : ds->m_Entities) {
		DialogueHUD* dialogueHudData = &dialogueHudArray.GetData(entity);
		if (!dialogueHudData->currentDialogue->dialogueLines.size() && (dialogueHudData->currentDialogue->targetScene != "")) {
			events.Call("Transition Scene", dialogueHudData->currentDialogue->targetScene);
			break;
		}
		if (!dialogueHudData->currentDialogue->isActive && !dialogueHudData->currentDialogue->isTriggered) {
			dialogueHudData->currentDialogue->viewingIndex = 0;
			dialogueHudData->StartDialogue(entity, DIALOGUE_TRIGGER::EVENT_BASED);
		}
	}
}

/*!
 * \brief Explicit event trigger to move on to next scene of dialogue.
 *
 * std::string input : The input string.
 */
void AdvanceDialogue(std::string input) {
	(void)input;

	UIDialogueSystem* ds = events.GetDialogueSystem();
	auto& dialogueHudArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<DialogueHUD>() };

	for (Entity const& entity : ds->m_Entities) {
		DialogueHUD* dialogueHudData = &dialogueHudArray.GetData(entity);
		if (dialogueHudData->currentDialogue->dialogueLines.size()) {
			dialogueHudData->JumpNextLine(entity);
		}
	}
}

/*!
 * \brief Initializes the functions for the event manager.
 *
 * Populates the functions map with predefined function pointers and builds
 * a vector of function names.
 */
void EventManager::InitialiseFunctions() {
	//functions["ChangeLevel"] = ChangeLevel;
	functions["Play Sound"] = PlayAudio;
	functions["Play Music"] = PlayMusic;
	functions["Restart Music"] = RestartMusic;
	functions["Pause/Resume Group"] = PauseResumeGroup;
	functions["Stop Group"] = StopGroup;
	functions["Select Skill"] = SelectSkill;
	functions["Select Enemy"] = SelectEnemy;
	functions["Toggle Pause"] = TogglePause;
	functions["Exit Game"] = ExitGame;
	functions["Change Scene"] = ChangeScene;
	functions["Transition Scene"] = TransitionScene;
	functions["Toggle Help"] = ToggleHelp;
	functions["Confirm Exit"] = ConfirmExit;
	functions["Test"] = TestFunction;
	functions["Start Dialogue"] = StartDialogue;
	functions["Advance Dialogue"] = AdvanceDialogue;
	for (auto& e : functions) {
		functionNames.push_back(e.first.c_str());
	}
}

/*!
 * \brief Calls the specified event function with the given input.
 *
 * \param functionName The name of the event function to be called.
 * \param functionInput The input string for the event function.
 */
void EventManager::Call(std::string functionName, std::string functionInput) {
	ASSERT(!functions.count(functionName), "Event function name is not valid!");
	functions[functionName](functionInput);
}


/*!
 * \brief Retrieves a vector containing the names of all available event functions.
 *
 * \return A vector of const char* containing the function names.
 */
std::vector<const char*> EventManager::GetFunctionNames() {
	return functionNames;
}

/*!
 * \brief Connects the event manager to a BattleSystem instance.
 *
 * \param input A pointer to the BattleSystem instance to be connected.
 */
void EventManager::ConnectBattleSystem(BattleSystem* input) {
	battlesystem = input;
}

/*!
 * \brief Retrieves the connected BattleSystem instance.
 *
 * \return A pointer to the connected BattleSystem instance.
 */
BattleSystem* EventManager::GetBattleSystem() {
	return battlesystem;
}

/*!
 * \brief Connects the event manager to a BattleSystem instance.
 *
 * \param input A pointer to the BattleSystem instance to be connected.
 */
void EventManager::ConnectDialogueSystem(UIDialogueSystem* input) {
	dialogueSystem = input;
}

/*!
 * \brief Retrieves the connected BattleSystem instance.
 *
 * \return A pointer to the connected BattleSystem instance.
 */
UIDialogueSystem* EventManager::GetDialogueSystem() {
	return dialogueSystem;
}
