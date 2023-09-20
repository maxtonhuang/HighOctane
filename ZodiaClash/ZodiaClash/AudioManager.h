#pragma once
#include <fmod/core/inc/fmod.hpp>
#include <unordered_map>
#include <string>

class AudioManager {
public:
	void Initialize();
	void AddSound(const char* path);
	void PlaySounds(const char* sound);
private:
	FMOD::System* system{};
	std::unordered_map<std::string, FMOD::Sound*> data{};
};

extern AudioManager audio;
