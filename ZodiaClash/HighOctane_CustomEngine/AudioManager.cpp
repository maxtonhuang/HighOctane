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
*	Audio manager loads and unloads all audio files as well as
*   stores them in a central map
*
******************************************************************************/

#include "AudioManager.h"
#include "DebugDiagnostic.h"
#include <iostream>

void AudioManager::Initialize() {
    FMOD_RESULT result;

    result = FMOD::System_Create(&system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        ASSERT(1, "Unable to create FMOD system!");
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        ASSERT(1, "Unable to initialise FMOD system!");
    }
    CreateGroup("SFX");
    CreateGroup("BGM");
}

void AudioManager::Release() {
    system->release();
}

FMOD::ChannelGroup* AudioManager::CreateGroup(const char* name) {
    if (group.count(name)) {
        return group[name];
    }
    FMOD::ChannelGroup* channelgroup;
    system->createChannelGroup(name, &channelgroup);
    group[name] = channelgroup;
    return group[name];
}

void AudioManager::SetGroupVolume(const char* name, float volume) {
    group[name]->setVolume(volume);
}

void AudioManager::StopGroup(const char* name) {
    group[name]->stop();
}

void AudioManager::ResumeGroup(const char* name) {
    group[name]->setPaused(false);
}

void AudioManager::PauseGroup(const char* name) {
    group[name]->setPaused(true);
}

bool AudioManager::IsGroupPaused(const char* name) {
    bool status;
    group[name]->getPaused(&status);
    return status;
}

FMOD::Sound* AudioManager::AddSound(const char* path, const char* name) {
    if (data.count(name)) {
        return data[name];
    }
    FMOD_RESULT result;
    result = system->createSound(path, FMOD_DEFAULT,0,&data[name]);
    if (result != FMOD_OK) {
        ASSERT(1, "Error creating sound!");
    }
    return data[name];
}

FMOD::Sound* AudioManager::AddMusic(const char* path, const char* name) {
    if (data.count(name)) {
        return data[name];
    }
    FMOD_RESULT result;
    result = system->createSound(path, FMOD_LOOP_NORMAL, 0, &data[name]);
    if (result != FMOD_OK) {
        ASSERT(1, "Error creating music!");
    }
    return data[name];
}

void AudioManager::PlaySounds(const char* sound, const char* channelGroup) {
    FMOD::Channel* tmp;
    if (channelGroup == nullptr) {
        system->playSound(data[sound], 0, false, &tmp);
    }
    else {
        system->playSound(data[sound], group[channelGroup], false, &tmp);
    }
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

FMOD::System* AudioManager::GetSystem() {
    return system;
}
