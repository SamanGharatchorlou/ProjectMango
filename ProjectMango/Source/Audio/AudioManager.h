#pragma once

#include "SoundController.h"

struct AudioEvent
{
	enum Action
	{
		Play,
		Loop,
		Pause,
		Resume,
		Stop,
		FadeIn,
		FadeInMusic,
		FadeOut,
	};

	AudioEvent(Action _action, const char* _label, const void* _id)								: action(_action), label(_label), id(reinterpret_cast<const uintptr_t>(_id)), source(VectorF(-1.0f, -1.0f)), time(-1) { }
	AudioEvent(Action _action, const char* _label, const void* _id, VectorF _source)			: action(_action), label(_label), id(reinterpret_cast<const uintptr_t>(_id)), source(_source),				 time(-1) { }
	AudioEvent(Action _action, const char* _label, const void* _id, int _time)					: action(_action), label(_label), id(reinterpret_cast<const uintptr_t>(_id)), source(VectorF(-1.0f, -1.0f)), time(_time) { }
	AudioEvent(Action _action, const char* _label, const void* _id, VectorF _source, int _time)	: action(_action), label(_label), id(reinterpret_cast<const uintptr_t>(_id)), source(_source),				 time(_time) { }

	const Action action;
	const StringBuffer64 label;
	const uintptr_t id;
	const VectorF source;
	const int time;
};



class AudioManager
{
public:
	static AudioManager* Get();

	AudioManager();
	~AudioManager();

	void resetController() { mSoundController.clear(); }
	void init();
	void load();
	void unload();

	void preLoad();

	//void setSource(Actor* listener, float attenuationDistance);
	void Update();

	Audio* getAudio(const char* label) const;
	const char* getLabel(Audio* audio) const;

	void push(AudioEvent event);

	bool isPlaying(const char* label, const void* sourced) const;
	bool isActive(const char* label, const void* sourced) const;

	// volume
	void setSoundVolume(float volume);
	float soundVolume() const;

	void setMusicVolume(float volume);
	float musicVolume() const;

	void toggleMute();
	void mute(bool shouldMute);


private:
	int loadAllMusic(FileManager::Folder folder);
	int loadAllSound(FileManager::Folder folder);
	int loadAllSoundGroups(FileManager::Folder folder);

	bool loadAudio(Audio* audio, const char* name, const char* filePath);

private:
	SoundController mSoundController;
	std::unordered_map<StringBuffer64, Audio*> mAudioBank;
};