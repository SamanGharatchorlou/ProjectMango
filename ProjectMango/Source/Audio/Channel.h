#pragma once

//class Audio;
//enum class AudioType;
//
//struct Channel
//{
//	//enum State
//	//{
//	//	Free,
//	//	Playing,
//	//	//Looping, // remove this, replace with a bool?
//	//	Paused
//	//};
//
//	Channel();
//
//	void play();
//	//void loop();
//	//void playNext();
//
//	void stop();
//	void resume();
//	void pause();
//
//	void fadeIn(int ms);
//	void fadeOut(int ms);
//
//	bool isPaused() const;
//	bool hasPlayingState() const;
//	bool isAudioRunning() const;
//
//	bool has(const Audio* audio, uintptr_t id) const;
//	bool isPlaying(const Audio* audio, uintptr_t id) const;
//
//	bool ShouldAttenuate() const;
//
//	bool canFree() const;
//	void free();
//
//	AudioType type();
//
//	State state;
//	State stateOnResume;
//
//	VectorF mSource;
//	//uintptr_t mID;
//
//	// 0 - 1 independant of game sound volume
//	float mVolume;
//
//	int index;
//	//const Audio* audio;
//
//	Sound sound;
//};