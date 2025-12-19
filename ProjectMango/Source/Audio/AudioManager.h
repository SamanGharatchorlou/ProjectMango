#pragma once

#include "SoundController.h"


class AudioManager
{
public:
	static AudioManager* Get();
	static SoundController* GetController();

	AudioManager();
	~AudioManager();

	void load();
	//void unload();

	void preLoad();

	//void setSource(Actor* listener, float attenuationDistance);
	//void Update();
	
	void PlayMusic(const char* id);
	void PlaySoundEffect(const char* id, int time = -1);
	
	Mix_Chunk* GetSoundEffectNoError(const char* id) const;
	Mix_Chunk* GetSoundEffect(const char* id) const;
	const char* GetSoundEffectId(const Mix_Chunk* sound_effect) const;

	Mix_Music* GetMusicTrack(const char* id);

	//void push(AudioEvent event);

	//bool isPlaying(const char* label, const void* sourced) const;
	//bool isActive(const char* label, const void* sourced) const;

	// volume
	//void setSoundVolume(float volume);
	//float soundVolume() const;

	//void setMusicVolume(float volume);
	//float musicVolume() const;

	//void toggleMute();
	//void mute(bool shouldMute);
	
	SoundController mSoundController;

private:
	int LoadAllSoundEffects();
	void FreeAllSoundEffects();

	bool LoadSoundEffect(const char* name, const char* filePath);
	Mix_Music* LoadMusicTrack(const char* name, const char* filePath);

private:

	std::unordered_map<StringBuffer64, Mix_Chunk*> soundEffects;
	std::unordered_map<StringBuffer64, Mix_Music*> musicTracks;
};