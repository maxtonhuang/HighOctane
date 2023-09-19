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
