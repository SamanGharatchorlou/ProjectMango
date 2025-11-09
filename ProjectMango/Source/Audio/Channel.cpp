#include "pch.h"
#include "Channel.h"

#include "Audio.h"


//Channel::Channel() : index(-1)
//{
//	free();
//}
//
//void Channel::free()
//{
//	//state = Free;
//	audio = nullptr;
//	//mID = NULL;
//	mSource = VectorF(-1.0f, -1.0f);
//	mVolume = 1.0f;
//}


//bool Channel::hasPlayingState() const
//{
//	return state == Playing;// || state == Looping;
//}





//bool Channel::isPaused() const
//{
//	return state == Paused;
//}

//// continue loop
//void Channel::playNext()
//{
//	audio->playNext(mIndex);
//}

// start loop
//void Channel::loop()
//{
//	state = Looping;
//	audio->play(index);
//
//#if PRINT_PLAYING_AUDIO
//	DebugPrint(Log, "%d: Looping audio: %s", mIndex, mAudio->name().c_str());
//#endif
//}

//void Channel::play()
//{
//	//state = Playing;
//	sound.Play(mIndex);
//
//#if PRINT_PLAYING_AUDIO
//	DebugPrint(Log, "%d: Playing audio: %s", mIndex, mAudio->name().c_str());
//#endif
//}
//
//
//void Channel::fadeIn(int ms)
//{
//	state = Playing;
//	mAudio->fadeIn(mIndex, ms);
//#if PRINT_PLAYING_AUDIO
//	DebugPrint(Log, "%d: Fading in audio: %s", mIndex, mAudio->name().c_str());
//#endif
//}
//
//
//void Channel::fadeOut(int ms)
//{
//	state = Playing;
//	mAudio->fadeOut(mIndex, ms);
//#if PRINT_PLAYING_AUDIO
//	DebugPrint(Log, "%d: Fading out audio: %s", mIndex, mAudio->name().c_str());
//#endif
//}


//void Channel::stop()
//{
//	mAudio->stop(mIndex);
//}
//
//void Channel::resume()
//{
//	state = stateOnResume;
//	stateOnResume = Free;
//	mAudio->resume(mIndex);
//}
//
//void Channel::pause()
//{
//	stateOnResume = state;
//	state = Paused;
//	mAudio->pause(mIndex);
//}

//bool Channel::isAudioRunning() const
//{
//	return mAudio->isPlaying(mIndex);
//}
//
//
//bool Channel::isPlaying(const Audio* audio, uintptr_t id) const
//{
//	return hasPlayingState() && has(audio, id);
//}
//
//bool Channel::has(const Audio* audio, uintptr_t id) const
//{
//	return mID == id && mAudio == audio;
//}
//
//bool Channel::canFree() const
//{
//	return hasPlayingState() && !isAudioRunning();
//}

//AudioType Channel::type() { return mAudio->type(); }