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
*	@file		AudioManager.h
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
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief		Audio manager
*
*   Contains functions to play audio
*	Audio manager loads and unloads all audio files as well as stores them in a central map
*
******************************************************************************/

#pragma once
#include <fmod/core/inc/fmod.hpp>
#include <unordered_map>
#include <string>

const float LOW_FILTER_VALUE{ 0.5f };

class AudioManager {
public:
	void Initialize(); //Creates FMOD system
	void Update(); //Updates FMOD, to be called every frame
	void Release(); //DELETES THE AUDIO SYSTEM, ONLY CALL AT END OF PROGRAM

	//FUNCTIONS FOR CHANNEL GROUPS
	//Creates a group with input name
	FMOD::ChannelGroup* CreateGroup(const char* name); 
	//Sets the volume of the group
	void SetGroupVolume(const char* name, float volume); 
	//Get the group volume, float* is the input volume
	float GetGroupVolume(const char* name);
	//Sets the low pass filter of the group
	void SetGroupFilter(const char* name, float filter);
	//Stops all sounds in the group
	void StopGroup(const char* name);
	//Resumes the group
	void ResumeGroup(const char* name); 
	//Pauses the group
	void PauseGroup(const char* name); 
	//Returns true if group is paused
	bool IsGroupPaused(const char* name); 
	//Sets the BGM of the scene
	void SetBGM(const char* name); 
	//Restarts scene BGM to original
	void RestartBGM();
	//Sets the ambience of the scene
	void SetAmbience(const char* name);

	//Releases all sounds from audio manager
	void ReleaseAllSounds(); 
	//Add a sound to FMOD and audio manager
	FMOD::Sound* AddSound(const char* path, const char* name); 
	//Add music to FMOD and audio manager
	FMOD::Sound* AddMusic(const char* path, const char* name); 
	//Add ambience to FMOD and audio manager
	FMOD::Sound* AddAmbience(const char* path, const char* name); 
	//Plays loaded sound
	FMOD::Channel* PlaySounds(const char* sound, const char* channelGroup = nullptr);
	//Free a sound from FMOD and audio manager
	void FreeSound(const char* sound); 
	//Gets the FMOD system
	FMOD::System* GetSystem(); 

	//Get names of all sounds currently loaded
	std::vector<std::string> GetSoundNames(); 
	//Get paths of all sounds in sound folder
	std::vector<std::string> GetSoundPaths(); 
	//Get paths of all music in music folder
	std::vector<std::string> GetMusicPaths(); 
	//Get paths of all ambience in ambience folder
	std::vector<std::string> GetAmbiencePaths();
	//Get name of the current BGM
	std::string GetCurrentBGM(); 
	//Get name of the current ambience
	std::string GetCurrentAmbience();
	//Update the paths in sound and music folders
	void UpdateAudioDirectory(); 
private:
	FMOD::System* system{};
	std::unordered_map<std::string, FMOD::Sound*> data{};
	std::unordered_map<std::string, FMOD::ChannelGroup*> group{};
	std::unordered_map<std::string, std::list<FMOD::Channel*>> channels{};

	std::string currentBGM{};
	std::string originalBGM{};
	std::string currentAmbience{};
	std::vector<std::string> soundPaths{};
	std::vector<std::string> musicPaths{};
	std::vector<std::string> ambiencePaths{};
};
