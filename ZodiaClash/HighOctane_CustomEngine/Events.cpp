#include "Events.h"
#include "AssetManager.h"

EventManager events;

void ChangeLevel(std::string input) {

}
void PlayAudio(std::string input) {
	//Find the entity from map using input string
	//Call the sound component and play it
	DEBUG_PRINT("playing: %s", input.c_str());
	assetmanager.audio.PlaySounds(input.c_str(), "SFX");
}
void TestFunction(std::string input) {
	std::cout << input << "\n";
}

void EventManager::InitialiseFunctions() {
	//functions["ChangeLevel"] = ChangeLevel;
	functions["Audio"] = PlayAudio;
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
