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
#include "Tutorial.h"

EventManager events;

Entity exitconfirmationmenu{};
Entity pausemenu{};
Entity settingsmenu{};
Entity scenemenu{};

/*!
 * \brief Exits the game menu, voids input
 *
 * std::string input : The input string.
 */
void ExitGame(std::string input) {
	static auto& nameArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Name>() };

	BattleSystem* bs{ events.GetBattleSystem() };

	if (exitconfirmationmenu) {
		//SetCurrentSystemMode(GetPreviousSystemMode());
		ECS::ecs().DestroyEntity(exitconfirmationmenu);
		exitconfirmationmenu = 0;

		if (bs->battleState == LOSE) {
			EntityFactory::entityFactory().ClonePrefab("losetext.prefab");
		}
		else if (!pausemenu && sceneName != "mainmenu.scn") {
			pausemenu = EntityFactory::entityFactory().ClonePrefab("pausemenu.prefab");
		}
	}
	else {
		//SetCurrentSystemMode(SystemMode::EXITCONFIRM);
		if (input == "Main Menu") {
			exitconfirmationmenu = EntityFactory::entityFactory().ClonePrefab("mainmenuconfirmation.prefab");
		}
		else {
			exitconfirmationmenu = EntityFactory::entityFactory().ClonePrefab("exitconfirmationmenu.prefab");
		}
		if (pausemenu) {
			EntityFactory::entityFactory().DeleteCloneModel(pausemenu);
			pausemenu = 0;
		}
		else if (bs->battleState == LOSE) {
			auto namePairs{ nameArray.GetPairArray() };
			for (auto& p : namePairs) {
				if (p.second->name == "Lose Screen") {
					EntityFactory::entityFactory().DeleteCloneModel(p.first);
				}
			}
		}
	}
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
			return;
		}
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
 * \brief Restarts the current scene
 *
 * std::string input : The input string.
 */
void RestartScene(std::string input) {
	(void)input;

	transitionActive = true;
	transitionNextScene = sceneName;
	transitionType = true;
}

/*!
 * \brief Plays the input audio name to SFX group
 *
 * std::string input : The input string.
 */
void PlayAudio(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	//DEBUG_PRINT("playing: %s", input.c_str());
	if (input == "") {
		return;
	}
	assetmanager.audio.PlaySounds(input.c_str(),"SFX");
}

void PlayVocal(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	//DEBUG_PRINT("playing: %s", input.c_str());
	if (input == "") {
		return;
	}
	assetmanager.audio.PlaySounds(input.c_str(), "VOC");
}

/*!
 * \brief Plays the input audio name to BGM group
 *
 * std::string input : The input string.
 */
void PlayMusic(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	//DEBUG_PRINT("playing: %s", input.c_str());
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
	//DEBUG_PRINT("playing: %s", input.c_str());
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
	if (skillnum < 1 || skillnum > static_cast<int>(bs->activeCharacter->action.skills.size())) {
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
	if (ECS::ecs().EntityExists(bs->battleinfo)) {
		EntityFactory::entityFactory().DeleteCloneModel(bs->battleinfo);
		bs->battleinfo = 0;
	}
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
	if (bs->activeCharacter->action.selectedSkill.attacktype == AttackType::ALLY || bs->activeCharacter->action.selectedSkill.attacktype == AttackType::ALLYSELF) {
		targets = bs->GetPlayers();
	}
	if (bs->activeCharacter->debuffs.tauntStack > 0 && bs->activeCharacter->action.selectedSkill.attacktype != AttackType::ALLY 
		&& bs->activeCharacter->action.selectedSkill.attacktype != AttackType::ALLYSELF) {
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
	
	if (ECS::ecs().EntityExists(bs->battleinfo)) {
		EntityFactory::entityFactory().DeleteCloneModel(bs->battleinfo);
		bs->battleinfo = 0;
	}

	bs->activeCharacter->action.entityState = ATTACKING;
	bs->DestroyTargets();
}

void ToggleBattleInfo(std::string input) {
	BattleSystem* bs = events.GetBattleSystem();
	UITutorialSystem* ts = events.GetTutorialSystem();

	if (ts->overlayOn) {
		return;
	}

	if (ECS::ecs().EntityExists(bs->battleinfo)) {
		EntityFactory::entityFactory().DeleteCloneModel(bs->battleinfo);
		bs->battleinfo = 0;
		return;
	}
	else {
		bs->battleinfo = EntityFactory::entityFactory().ClonePrefab(input);
	}
}

/*!
 * \brief Toggling of the pause state of the game.
 *
 * std::string input : The input string.
 */
void TogglePause(std::string input) {

	if (sceneName == "mainmenu.scn") {
		return;
	}

	if (GetCurrentSystemMode() == SystemMode::GAMEHELP ||GetCurrentSystemMode() == SystemMode::SETTINGS || GetCurrentSystemMode() == SystemMode::EDIT) {

		return;
	}

	BattleSystem* bs = events.GetBattleSystem();
	if (bs->battleState == LOSE) {
		return;
	}

	(void)input;
	UITutorialSystem* ts = events.GetTutorialSystem();

	PauseResumeGroup("VOC");
	/*-----Prevent Softlocking-----*/
	if ((GetPreviousSystemMode() == SystemMode::GAMEHELP || GetPreviousSystemMode() == SystemMode::SETTINGS) && GetCurrentSystemMode() == SystemMode::PAUSE) {
		SetCurrentSystemMode(SystemMode::RUN);
		if (pausemenu != 0) {
			EntityFactory::entityFactory().DeleteCloneModel(pausemenu);
			pausemenu = 0;
		}
	}
	/*-----Prevent Softlocking-----*/

	else if ((!(GetPreviousSystemMode() == SystemMode::GAMEHELP) || !(GetPreviousSystemMode() == SystemMode::SETTINGS)) && GetCurrentSystemMode() == SystemMode::PAUSE) {
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
			
			if (ts->overlay)
				ts->SurfaceSystemOverlay(pausemenu);
		}
	}

	// reset tutorial system overlay flag
	if (GetCurrentSystemMode() != SystemMode::PAUSE && ts->systemOverlayOn)
		ts->systemOverlayOn = false;
}

// Add in toggle settings in events which is here.


/*!
 * \brief Toggling of the settings state of the game.
 *
 * std::string input : The input string. (Not used)
 */
void ToggleSettings(std::string input) {
	if (GetCurrentSystemMode() == SystemMode::PAUSE) {
		SetCurrentSystemMode(SystemMode::SETTINGS);
		settingsmenu = EntityFactory::entityFactory().ClonePrefab("settingsmenu.prefab");
	}
	else if (GetCurrentSystemMode() == SystemMode::SETTINGS) {
		SetCurrentSystemMode(SystemMode::PAUSE);
		if (settingsmenu != 0) {
			EntityFactory::entityFactory().DeleteCloneModel(settingsmenu);
			settingsmenu = 0;
		}
		UITutorialSystem* ts = events.GetTutorialSystem();
		if (ts->systemOverlayOn) {
			ts->MaintainLayers();
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

		if (GetCurrentSystemMode() == SystemMode::PAUSE) {
			UITutorialSystem* ts = events.GetTutorialSystem();
			if (ts->systemOverlayOn) {
				ts->MaintainLayers();
			}
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
 * \brief Toggling of the scene of the game.
 *
 * std::string input : The input string.
 */
void ToggleScene(std::string input) {
	(void)input;

	if (GetCurrentSystemMode() == SystemMode::PAUSE || GetCurrentSystemMode() == SystemMode::GAMEHELP || GetCurrentSystemMode() == SystemMode::SETTINGS || GetCurrentSystemMode() == SystemMode::EDIT) {

		return;
	}

	if (scenemenu != 0) {
		EntityFactory::entityFactory().DeleteCloneModel(scenemenu);
		scenemenu = 0;
	}
	else {
		scenemenu = EntityFactory::entityFactory().ClonePrefab("scene_select.prefab");
	}
	UITutorialSystem* ts = events.GetTutorialSystem();
	if (ts->systemOverlayOn) {
		ts->MaintainLayers();
	}
}

/*!
 * \brief Transitions from current scene to target scene.
 *
 * std::string input : The input string.
 */
void TransitionScene(std::string input) {
	if (exitconfirmationmenu) {
		//SetCurrentSystemMode(GetPreviousSystemMode());
		TogglePause("");
		ECS::ecs().DestroyEntity(exitconfirmationmenu);
		exitconfirmationmenu = 0;
	}
	if (scenemenu) {
		ECS::ecs().DestroyEntity(scenemenu);
		scenemenu = 0;
	}

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
		if (dialogueHudData->currentDialogue && !dialogueHudData->currentDialogue->dialogueLines.size() && (dialogueHudData->currentDialogue->targetScene != "")) {
			events.Call("Transition Scene", dialogueHudData->currentDialogue->targetScene);
			break;
		}

		// check input
		DIALOGUE_TRIGGER triggerEnum;
		if (input == "EVENT")
			triggerEnum = DIALOGUE_TRIGGER::EVENT_BASED;
		else if (input == "PRE_BATTLE")
			triggerEnum = DIALOGUE_TRIGGER::PRE_BATTLE;
		else if (input == "TURN")
			triggerEnum = DIALOGUE_TRIGGER::TURN_BASED;
		else if (input == "HEALTH")
			triggerEnum = DIALOGUE_TRIGGER::HEALTH_BASED;
		else if (input == "WIN")
			triggerEnum = DIALOGUE_TRIGGER::POST_BATTLE_WIN;
		else if (input == "LOSE")
			triggerEnum = DIALOGUE_TRIGGER::POST_BATTLE_LOSE;
		else
			triggerEnum = DIALOGUE_TRIGGER::DEFAULT;

		dialogueHudData->StartDialogue(entity, triggerEnum);
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
 * \brief Event trigger to start tutorial in battle.scn
 *
 * std::string input : The input string.
 */
void StartTutorial(std::string input) {
	(void)input;
	UITutorialSystem* ts = events.GetTutorialSystem();
	BattleSystem* bs = events.GetBattleSystem();
	if (!ts->overlay) {
		//Create overlay prefab here
		ts->overlay = EntityFactory::entityFactory().ClonePrefab("tutorial_overlay.prefab");
		ts->stepIndex = 0;
		ts->UpdateState();

		bs->tutorialLock = 1;
	}
}

/*!
 * \brief Event trigger to advance tutorial in battle.scn
 *
 * std::string input : The input string.
 */
void AdvanceTutorial(std::string input) {
	UITutorialSystem* ts = events.GetTutorialSystem();
	bool conditionFulfilled = true;
	ts->CheckConditionFulfilled(conditionFulfilled);
	if (!conditionFulfilled) {
		return;
	}
	ts->stepIndex++;
	if (ts->currentTutorialEntity) {
		EntityFactory::entityFactory().DeleteCloneModel(ts->currentTutorialEntity);
		ts->currentTutorialEntity = 0;
	}
	ts->UpdateState();
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
	functions["Play Voice"] = PlayVocal;
	functions["Pause/Resume Group"] = PauseResumeGroup;
	functions["Stop Group"] = StopGroup;
	functions["Select Skill"] = SelectSkill;
	functions["Select Enemy"] = SelectEnemy;
	functions["Toggle Battle Info"] = ToggleBattleInfo;
	functions["Toggle Pause"] = TogglePause;
	functions["Exit Game"] = ExitGame;
	functions["Change Scene"] = ChangeScene;
	functions["Restart Scene"] = RestartScene;
	functions["Transition Scene"] = TransitionScene;
	functions["Toggle Help"] = ToggleHelp;
	functions["Confirm Exit"] = ConfirmExit;
	functions["Test"] = TestFunction;
	functions["Start Dialogue"] = StartDialogue;
	functions["Advance Dialogue"] = AdvanceDialogue;
	functions["Start Tutorial"] = StartTutorial;
	functions["Advance Tutorial"] = AdvanceTutorial;
	functions["Toggle Settings"] = ToggleSettings;
	functions["Toggle Scene"] = ToggleScene;
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
 * \brief Retrieves the connected UIDialogueSystem instance.
 *
 * \return A pointer to the connected UIDialogueSystem instance.
 */
UIDialogueSystem* EventManager::GetDialogueSystem() {
	return dialogueSystem;
}

/*!
 * \brief Connects the event manager to a UITutorialSystem instance.
 *
 * \param input A pointer to the UITutorialSystem instance to be connected.
 */
void EventManager::ConnectTutorialSystem(UITutorialSystem* input) {
	tutorialSystem = input;
}

/*!
 * \brief Retrieves the connected UITutorialSystem instance.
 *
 * \return A pointer to the connected UITutorialSystem instance.
 */
UITutorialSystem* EventManager::GetTutorialSystem() {
	return tutorialSystem;
}
