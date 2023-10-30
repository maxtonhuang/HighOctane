#include "Events.h"
#include "AssetManager.h"
#include "enginecore.h"
#include "Global.h"

EventManager events;

void ExitGame(std::string input) {
	(void)input;
	EngineCore::engineCore().setGameActive(false);
}

void ChangeScene(std::string input) {
	newScene = true;
	button_clicked = true;
	newSceneName = input;
	destroyAll = true;
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
	functions["Exit Game"] = ExitGame;
	functions["Change Scene"] = ChangeScene;
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
