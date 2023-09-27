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
*	@file		AudioManager.cpp
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
*	Audio manager loads and unloads all audio files as well as
*   stores them in a central map
*
******************************************************************************/

#include "AudioManager.h"
#include <iostream>

AudioManager audio;

void AudioManager::Initialize() {
    FMOD_RESULT result;

    result = FMOD::System_Create(&system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        std::cout << "Error!\n";
        //printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        std::cout << "Error!\n";
        //printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
    }
}

void AudioManager::Release() {
    system->release();
}

void AudioManager::AddSound(const char* path) {
    FMOD_RESULT result;
    result = system->createSound(path, FMOD_DEFAULT,0,&data[path]);
    if (result != FMOD_OK) {
        std::cout << "Error creating sound!\n";
    }
}

void AudioManager::PlaySounds(const char* sound) {
    FMOD::Channel* tmp;
    system->playSound(data[sound], 0, false, &tmp);
}

void AudioManager::FreeSound(const char* sound) {
    data[sound]->release();
    data.erase(sound);
}

void AudioManager::ReleaseAllSounds() {
    for (auto const& sound : data) {
        sound.second->release();
    }
    data.clear();
}
