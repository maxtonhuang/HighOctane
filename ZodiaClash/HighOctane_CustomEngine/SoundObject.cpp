/*
CURRENTLY UNUSED SOUND COMPONENT
*/

#include "SoundObject.h"

SoundObject::SoundObject(const char* soundPath, const char* channelGroup) {
    sound = audio.AddSound(soundPath);
    group = audio.CreateGroup(channelGroup);
}

void SoundObject::Play() {
    audio.GetSystem()->playSound(sound, group, false, &channel);
}

void SoundObject::Update() {

}
