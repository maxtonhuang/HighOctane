/*
CURRENTLY UNUSED SOUND COMPONENT
*/

#pragma once

#include "AudioManager.h"

class SoundObject {
public:
	SoundObject(const char* soundPath, const char* channelGroup);
	void Play();
	void Update();
private:
	FMOD::Sound* sound{};
	FMOD::Channel* channel{};
	const char* soundName{};
	FMOD::ChannelGroup* group{};
};
