#include "FunctionPointer.h"
#include "AssetManager.h"

std::unordered_map <std::string, Event> functions;

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

void InitialiseFunctions() {
	functions["ChangeLevel"] = ChangeLevel;
	functions["Audio"] = PlayAudio;
	functions["Test"] = TestFunction;
}
