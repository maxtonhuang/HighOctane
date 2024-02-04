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
*	@file		Events.h
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

#pragma once
#include "ECS.h"
#include "Battle.h"

using Event = void(*)(std::string);

class EventManager {
public:
	/*
	AVAILABLE EVENTS:
	"Play Sound": Plays a sound using input sound file
	"Play Music": Plays music using input sound file
	"Restart Music": Stops all current music and plays input sound file
	"Pause/Resume Group": Pause sound group. If paused, resume it instead
	"Stop Group": Stops the sound group
	"Select Skill": Takes in a number and selects that skill number for current player character
	"Exit Game": Exits the game entirely
	"Change Scene": Changes scene using input scene file
	"Test": Prints input
	*/
	void Call(std::string functionName, std::string functionInput);
	void InitialiseFunctions();
	std::vector<const char*> GetFunctionNames();

	void ConnectBattleSystem(BattleSystem* input);
	BattleSystem* GetBattleSystem();

	void ConnectDialogueSystem(UIDialogueSystem* input);
	UIDialogueSystem* GetDialogueSystem();
private:
	std::unordered_map<std::string, Event> functions;
	std::vector<const char*> functionNames;
	BattleSystem* battlesystem; //Connection to battle system
	UIDialogueSystem* dialogueSystem; //Connection to dialogue system
};

extern EventManager events;