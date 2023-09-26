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
*	@email		p.foong@digipen.edu
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
	void Initialize();
	void Release(); //DELETES THE AUDIO SYSTEM, ONLY CALL AT END OF PROGRAM
	void ReleaseAllSounds(); //Releases all sounds from audio manager
	void AddSound(const char* path); //Add a sound to FMOD and audio manager
	void PlaySounds(const char* sound); //Plays loaded sound
	void FreeSound(const char* sound); //Free a sound from FMOD and audio manager
private:
	FMOD::System* system{};
	std::unordered_map<std::string, FMOD::Sound*> data{};
};

extern AudioManager audio;
