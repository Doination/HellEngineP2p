#pragma once
#include <unordered_map>
#include "fmod.hpp"
#include <fmod_errors.h>
#include <string>
#include <iostream>
#include "cstdint"

struct AudioHandle {
	FMOD::Sound* sound = nullptr;
	FMOD::Channel* channel = nullptr;
    std::string filename;
    float fadeOutDuration = -1.0f;
    float fadeOutTimeRemaining = 0.0f;
    float decay = 0.0f;

    enum class State { 
        PLAYING,
        LOOPING,
        PAUSED,
        FADING_OUT 
    } state = State::PLAYING;
};

struct AudioEffectInfo {
	std::string filename = "";
	float volume = 0.0f;
};

namespace Audio {
    void Init();
    void Update(float deltaTime);
    void LoadAudio(const std::string& filename);
    void StopAudio(const std::string& filename);
    uint64_t LoopAudio(const std::string& filename, float volume);
    uint64_t LoopAudioIfNotPlaying(const std::string& filename, float volume);
    uint64_t PlayAudio(const std::string& filename, float volume, float frequency = 1.0f);

    void StopAudio(uint64_t audioId); 
    bool AudioIsPlaying(uint64_t audioId);
    void FadeOut(uint64_t audioId, float duration);

    float GetVolume(uint64_t audioId);
    void SetVolume(uint64_t audioId, float volume);
};