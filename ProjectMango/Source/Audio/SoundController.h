#pragma once

#include "Audio.h"

static int constexpr c_mixerChannels = 8;
float constexpr maxVolume = (float)MIX_MAX_VOLUME;

struct Channel
{
	//Sound sound;
	
	Mix_Chunk* sound;
	VectorF sourcePosition = c_invalidVector;

	// 0 - 1 independant of game sound volume
	float volume = 1.0f;
	int index = -1;

	// game time we started playing
	u32 playedAtTick = 0;
};

//// --- Channel --- //
//bool Channel::ShouldAttenuate() const
//{
//	return sourcePosition != c_invalidVector;
//}

struct SoundController
{
	SoundController();

	void ClearChannels();

	//void init();
	//void attenuationDistance(float maxDistance) { mAttenuationDistance = maxDistance * maxDistance; }
	//void setListener(Actor* listener) { mListener = listener; }

	//void Update();

	// note need to pass in an ID, play will replay the same audio, but that only
	// makes sense if the audio is from the same entity
	bool Play(Mix_Chunk* sound, int time, const char* id = nullptr);
	bool Pause(Mix_Chunk* sound);
	bool Resume(Mix_Chunk* sound);
	bool Stop(Mix_Chunk* sound);

	//void fadeIn(const Audio* audio, uintptr_t id, int ms, VectorF source);
	//void fadeInMusic(const Audio* audio, uintptr_t id, int ms);
	//void fadeOut(const Audio* audio, uintptr_t id, int ms);

	//bool hasActiveAudio(const Audio* audio, uintptr_t id) const;
	bool IsPlaying(Mix_Chunk* sound) const;
	bool IsPlaying(Channel channel) const;

	void SetSoundVolume(float volume);
	//float getSoundVolume() const { return soundVolume; }

	void SetMusicVolume(float volume);
	//float getMusicVolume() const { return musicVolume; }

	//void updateMixerVolume(Channel& channel);
	//float attenuation(Channel& channel);



	float soundVolume;
	float musicVolume;

	//float mAttenuationDistance;

	Channel channels[c_mixerChannels];
};



