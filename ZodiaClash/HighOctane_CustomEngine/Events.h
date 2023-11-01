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
private:
	std::unordered_map<std::string, Event> functions;
	std::vector<const char*> functionNames;
	BattleSystem* battlesystem; //Connection to battle system
};

extern EventManager events;