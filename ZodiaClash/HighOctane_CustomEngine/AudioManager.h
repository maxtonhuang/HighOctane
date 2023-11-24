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

class AudioManager {
public:
	void Initialize(); //Creates FMOD system
	void Update();
	void Release(); //DELETES THE AUDIO SYSTEM, ONLY CALL AT END OF PROGRAM

	//FUNCTIONS FOR CHANNEL GROUPS
	FMOD::ChannelGroup* CreateGroup(const char* name);
	void SetGroupVolume(const char* name, float volume);
	void StopGroup(const char* name);
	void ResumeGroup(const char* name);
	void PauseGroup(const char* name);
	bool IsGroupPaused(const char* name);
	void SetBGM(const char* name);

	void ReleaseAllSounds(); //Releases all sounds from audio manager
	FMOD::Sound* AddSound(const char* path, const char* name); //Add a sound to FMOD and audio manager
	FMOD::Sound* AddMusic(const char* path, const char* name);
	void PlaySounds(const char* sound, const char* channelGroup = nullptr); //Plays loaded sound
	void FreeSound(const char* sound); //Free a sound from FMOD and audio manager
	FMOD::System* GetSystem();

	std::vector<std::string> GetSoundNames();
	std::vector<std::string> GetSoundPaths();
	std::vector<std::string> GetMusicPaths();
	std::string GetCurrentBGM();
	void UpdateAudioDirectory();
private:
	FMOD::System* system{};
	std::unordered_map<std::string, FMOD::Sound*> data{};
	std::unordered_map<std::string, FMOD::ChannelGroup*> group{};

	std::string currentBGM{};
	std::vector<std::string> soundPaths{};
	std::vector<std::string> musicPaths{};
};
