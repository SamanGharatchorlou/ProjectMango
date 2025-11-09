#pragma once

//struct Channel
//{
//	Channel() : index(-1), sourcePosition(c_invalidVector), volume(1.0f) { }
//	//Channel(int _index) : index(index), 
//
//	//bool isPaused() const;
//	//bool hasPlayingState() const;
//	//bool isAudioRunning() const;
//
//	//bool has(const Audio* audio, uintptr_t id) const;
//	//bool isPlaying(const Audio* audio, uintptr_t id) const;
//
//	bool ShouldAttenuate() const;
//
//	//bool canFree() const;
//	//void free();
//
//	//AudioType type();
//
//	//State state;
//	//State stateOnResume;
//
//	VectorF sourcePosition;
//	//uintptr_t mID;
//
//	// 0 - 1 independant of game sound volume
//	float volume;
//
//	int index ;
//	//const Audio* audio;
//
//	Sound sound;
//};
//
//struct Sound
//{
//	Sound() : chunk(nullptr), loops(0) { }
//	Sound(Mix_Chunk* _chunk, int _loops) : chunk(_chunk), loops(_loops) { }
//
//	void Play(int channel);
//	void FadeIn(int channel, int ms);
//	void FadeOut(int channel, int ms);
//
//	//void Resume(int channel);
//	//void Pause(int channel);
//	//void Stop(int channel);
//
//	//bool IsPlaying(int channel);
//
//	Mix_Chunk *chunk;
//	int loops;
//};
//
//class Music
//{
//public:
//	Music(Mix_Music* _music) : music(_music), loops(-1) { }
//
//	void Play();
//	void FadeIn(int ms);
//	void FadeOut(int ms);
//
//	void Resume();
//	void Pause();
//	void Stop();
//
//	bool IsPlaying();
//
//public:
//	Mix_Music *music;
//	int loops;
//};


