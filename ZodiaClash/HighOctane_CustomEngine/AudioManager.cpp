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
#include "AssetManager.h"
#include "DebugDiagnostic.h"
#include <iostream>
#include <filesystem>

void AudioManager::Initialize() {
    FMOD_RESULT result;

    result = FMOD::System_Create(&system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        ASSERT(1, "Unable to create FMOD system!");
    }

    result = system->init(512, FMOD_INIT_CHANNEL_LOWPASS, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        ASSERT(1, "Unable to initialise FMOD system!");
    }
    system->getMasterChannelGroup(&group["Master"]);
    CreateGroup("SFX");
    CreateGroup("BGM");
    CreateGroup("ENV");
    CreateGroup("VOC");

    UpdateAudioDirectory();
}

void AudioManager::Update() {
    system->update();
    for (auto& c : channels) {
        std::vector<FMOD::Channel*> removeList{};
        for (auto& s : c.second) {
            bool isPlaying;
            s->isPlaying(&isPlaying);
            if (!isPlaying) {
                removeList.push_back(s);
            }
        }
        for (auto& r : removeList) {
            c.second.remove(r);
        }
    }
}

void AudioManager::UpdateAudioDirectory() {
    std::filesystem::path soundFolder{ assetmanager.GetDefaultPath() + "Sound/" };
    std::vector <std::string> newSoundPaths{};
    std::vector <std::string> newMusicPaths{};
    std::vector <std::string> newAmbiencePaths{};
    for (auto& entry : std::filesystem::directory_iterator(soundFolder)) {
        std::string path{ entry.path().string() };
        path = path.substr(path.find_last_of("/") + 1);
        newSoundPaths.push_back(path);
    }
    std::filesystem::path musicFolder{ assetmanager.GetDefaultPath() + "Music/" };
    for (auto& entry : std::filesystem::directory_iterator(musicFolder)) {
        std::string path{ entry.path().string() };
        path = path.substr(path.find_last_of("/") + 1);
        newMusicPaths.push_back(path);
    }
    std::filesystem::path ambienceFolder{ assetmanager.GetDefaultPath() + "Ambience/" };
    for (auto& entry : std::filesystem::directory_iterator(ambienceFolder)) {
        std::string path{ entry.path().string() };
        path = path.substr(path.find_last_of("/") + 1);
        newAmbiencePaths.push_back(path);
    }
    soundPaths = newSoundPaths;
    musicPaths = newMusicPaths;
    ambiencePaths = newAmbiencePaths;
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

float AudioManager::GetGroupVolume(const char* name) {
    float volume{};
    group[name]->getVolume(&volume);
    return volume;
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
    if (data.count(name) && data[name] != nullptr) {
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
    if (data[name] != nullptr) {
        return data[name];
    }
    FMOD_RESULT result;
    result = system->createSound(path, FMOD_LOOP_NORMAL, 0, &data[name]);
    if (result != FMOD_OK) {
        ASSERT(1, "Error creating music!");
    }

    //If no BGM loaded, player current BGM
    if (originalBGM == "") {
        PlaySounds(name, "BGM");
        originalBGM = name;
        currentBGM = name;
    }
    return data[name];
}

FMOD::Sound* AudioManager::AddAmbience(const char* path, const char* name) {
    if (data[name] != nullptr) {
        return data[name];
    }
    FMOD_RESULT result;
    result = system->createSound(path, FMOD_LOOP_NORMAL, 0, &data[name]);
    if (result != FMOD_OK) {
        ASSERT(1, "Error creating ambience!");
    }

    //If no BGM loaded, player current BGM
    if (currentAmbience == "") {
        PlaySounds(name, "ENV");
        currentAmbience = name;
    }
    return data[name];
}

void AudioManager::PlaySounds(const char* sound, const char* channelGroup) {
    FMOD::Channel* tmp;
    if (data[sound] == nullptr) {
        assetmanager.LoadAssets(sound);
    }
    if (channelGroup == nullptr) {
        system->playSound(data[sound], 0, false, &tmp);
    }
    else {
        system->playSound(data[sound], group[channelGroup], false, &tmp);
    }
    channels[channelGroup].push_back(tmp);
}

void AudioManager::FreeSound(const char* sound) {
    data[sound]->release();
    data.erase(sound);
}

void AudioManager::SetBGM(const char* name) {
    StopGroup("BGM");
    FreeSound(currentBGM.c_str());
    currentBGM = name;
    PlaySounds(name, "BGM");
}

void AudioManager::RestartBGM() {
    SetBGM(originalBGM.c_str());
}

void AudioManager::SetAmbience(const char* name) {
    StopGroup("ENV");
    FreeSound(currentAmbience.c_str());
    currentAmbience = name;
    PlaySounds(name, "ENV");
}

void AudioManager::ReleaseAllSounds() {
    for (auto const& sound : data) {
        sound.second->release();
    }
    data.clear();
    currentBGM.clear();
    originalBGM.clear();
    currentAmbience.clear();
}

FMOD::System* AudioManager::GetSystem() {
    return system;
}

std::vector<std::string> AudioManager::GetSoundNames() {
    std::vector<std::string> output{};
    for (auto& name : data) {
        output.push_back(name.first);
    }
    return output;
}

std::vector<std::string> AudioManager::GetSoundPaths() {
    return soundPaths;
}

std::vector<std::string> AudioManager::GetMusicPaths() {
    return musicPaths;
}

std::vector<std::string> AudioManager::GetAmbiencePaths() {
    return ambiencePaths;
}

std::string AudioManager::GetCurrentBGM() {
    return currentBGM;
}

std::string AudioManager::GetCurrentAmbience() {
    return currentAmbience;
}

void AudioManager::SetGroupFilter(const char* name, float filter) {
    for (auto& c : channels[name]) {
        c->setLowPassGain(filter);
    }
}
