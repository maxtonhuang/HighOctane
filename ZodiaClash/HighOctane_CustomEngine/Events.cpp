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

EventManager events;

//Exits the game, voids input
void ExitGame(std::string input) {
	(void)input;
	EngineCore::engineCore().setGameActive(false);
}

//Changes the scene to the input scene name
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
	
	// Change the current system mode to run, probably shouldn't be here
	//currentSystemMode = SystemMode::RUN; // change to run mode

	/*RebuildLayeringAfterDeserialization();
	ExtractSkipLockAfterDeserialization();*/
	//playButton = true;
}

//Plays the input audio name to SFX group
void PlayAudio(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(),"SFX");
}

//Plays the input audio name to BGM group
void PlayMusic(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(), "BGM");
}

//Stops BGM and plays the input audio name as BGM instead
void RestartMusic(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	assetmanager.audio.StopGroup("BGM");
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(), "BGM");
}

//Toggles pause or resume for the input audio group
void PauseResumeGroup(std::string input) {
	if (assetmanager.audio.IsGroupPaused(input.c_str())) {
		assetmanager.audio.ResumeGroup(input.c_str());
	}
	else {
		assetmanager.audio.PauseGroup(input.c_str());
	}
}

//Stops all sounds in the input audio group
void StopGroup(std::string input) {
	assetmanager.audio.StopGroup(input.c_str());
}

//Selects a skill of input number
void SelectSkill(std::string input) {
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

	bs->activeCharacter->action.selectedSkill = bs->activeCharacter->action.skills[skillnum - 1];
	bs->CreateTargets();
	//auto targets = bs->GetEnemies();
	//bs->activeCharacter->action.targetSelect.selectedTarget = targets[0];
	//bs->activeCharacter->action.entityState = ATTACKING;
}

//Selects an enemy of input number
void SelectEnemy(std::string input) {
	BattleSystem* bs = events.GetBattleSystem();
	std::stringstream ss{ input };
	int enemynum;
	ss >> enemynum;

	auto targets = bs->GetEnemies();
	bs->activeCharacter->action.targetSelect.selectedTarget = targets[enemynum];
	bs->activeCharacter->action.entityState = ATTACKING;
	bs->DestroyTargets();
}
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

void TestFunction(std::string input) {
	std::cout << input << "\n";
}

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
	functions["Toggle Help"] = ToggleHelp;
	functions["Test"] = TestFunction;
	for (auto& e : functions) {
		functionNames.push_back(e.first.c_str());
	}
}

void EventManager::Call(std::string functionName, std::string functionInput) {
	ASSERT(!functions.count(functionName), "Event function name is not valid!");
	functions[functionName](functionInput);
}

std::vector<const char*> EventManager::GetFunctionNames() {
	return functionNames;
}

void EventManager::ConnectBattleSystem(BattleSystem* input) {
	battlesystem = input;
}

BattleSystem* EventManager::GetBattleSystem() {
	return battlesystem;
}
