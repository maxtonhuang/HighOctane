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

void ExitGame(std::string input) {
	(void)input;
	EngineCore::engineCore().setGameActive(false);
}

void ChangeScene(std::string input) {
	if (sceneName == input) {
		return;
	}

	newScene = true;
	button_clicked = true;
	newSceneName = input;
	destroyAll = true;
	/*RebuildLayeringAfterDeserialization();
	ExtractSkipLockAfterDeserialization();*/
	//playButton = true;
}
void PlayAudio(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(),"SFX");
}
void PlayMusic(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(), "BGM");
}
void RestartMusic(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	assetmanager.audio.StopGroup("BGM");
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(), "BGM");
}
void PauseResumeGroup(std::string input) {
	if (assetmanager.audio.IsGroupPaused(input.c_str())) {
		assetmanager.audio.ResumeGroup(input.c_str());
	}
	else {
		assetmanager.audio.PauseGroup(input.c_str());
	}
}
void StopGroup(std::string input) {
	assetmanager.audio.StopGroup(input.c_str());
}
void SelectSkill(std::string input) {
	BattleSystem* bs = events.GetBattleSystem();

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

	bs->activeCharacter->action.selectedSkill = bs->activeCharacter->action.skills[skillnum - 1];

	// check if the character has enough Chi to perform the skill
	BattleSystem* battlesys = events.GetBattleSystem();
	if (battlesys->chi >= bs->activeCharacter->action.selectedSkill.chiCost) {
		auto targets = bs->GetEnemies();
		bs->activeCharacter->action.targetSelect.selectedTarget = targets[0];
		bs->activeCharacter->action.entityState = ATTACKING;
	}
	else {
		// handle not enough Chi, ZR part?
	}

	auto targets = bs->GetEnemies();
	bs->activeCharacter->action.targetSelect.selectedTarget = targets[0];
	bs->activeCharacter->action.entityState = ATTACKING;
}
void TogglePause(std::string input) {
	if (currentSystemMode == SystemMode::GAMEHELP) {
		return;
	}
	(void)input;
	static Entity pausemenu{};
	if (currentSystemMode == SystemMode::PAUSE) {
		currentSystemMode = lastSystemMode;
		lastSystemMode = SystemMode::PAUSE;
		if (pausemenu != 0) {
			EntityFactory::entityFactory().DeleteCloneModel(pausemenu);
			pausemenu = 0;
		}
	}
	else {
		lastSystemMode = currentSystemMode;
		currentSystemMode = SystemMode::PAUSE;
		if (pausemenu == 0) {
			pausemenu = EntityFactory::entityFactory().ClonePrefab("pausemenu.prefab");
		}	
	}
}

void ToggleHelp(std::string input) {
	if (currentSystemMode == SystemMode::PAUSE) {
		return;
	}
	(void)input;
	static Entity gamehelpmenu{};
	if (currentSystemMode == SystemMode::GAMEHELP) {
		//if (lastSystemMode == SystemMode::PAUSE) {
		//	currentSystemMode = SystemMode::RUN;
		//}
		//else {
		//	currentSystemMode = lastSystemMode;

		//}
		currentSystemMode = lastSystemMode;
		lastSystemMode = SystemMode::GAMEHELP;
		if (gamehelpmenu != 0) {
			ECS::ecs().DestroyEntity(gamehelpmenu);
			gamehelpmenu = 0;
		}
	}
	else {
		lastSystemMode = currentSystemMode;
		currentSystemMode = SystemMode::GAMEHELP;
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
