#include "pch.h"
#include "SoundController.h"

#include "Audio.h"
//#include "Actors/Actor.h"


SoundController::SoundController() : soundVolume(-1), musicVolume(-1)
{
	Mix_AllocateChannels(mixerChannels);

	for (int i = 0; i < mixerChannels; i++)
	{
		channels[i].init(i);
	}


	clear();
}

void SoundController::clear()
{
	setMusicVolume(musicVolume);
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.audio())
			channel.stop();

		channel.free();
		updateMixerVolume(channel);
	}

	float vol = getMusicVolume();
	int vim = Mix_VolumeMusic(-1);
}

void SoundController::init()
{
	clear();
}


void SoundController::Update()
{
#if DEBUG_MODE
	int counter = 0;
#endif
	float vol = getMusicVolume();
	int vim = Mix_VolumeMusic(-1);

	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& sound = channels[i];

		if (sound.hasPlayingState() && sound.attenuate())
		{
			sound.mVolume = attenuation(sound);
		}

		if (sound.mState == Channel::Looping && !sound.isAudioRunning())
		{
			sound.playNext();
		}

		// Free up channels after audio has stopped playing
		if (sound.canFree())
		{
			sound.free();
		}

		updateMixerVolume(sound);

		float vol = getMusicVolume();
		int vim = Mix_VolumeMusic(-1);

#if DEBUG_MODE
		if (sound.mState != Channel::Free)
		{
			counter++;
		}
#endif
	}

#if DEBUG_MODE // Check channels aren't getting clogged up with paused audio
	if (counter >= mixerChannels)
	{
		int playingCounter = 0;
		int pauseCounter = 0;

		for (int i = 0; i < mixerChannels; i++)
		{
			const Channel& channel = channels[i];
			if (channel.hasPlayingState())
				playingCounter++;
			else if (channel.isPaused())
				pauseCounter++;
		}

		DebugPrint(Warning, "All the the 8 audio channels are being used. %d channels are playing and %d channels are paused", playingCounter, pauseCounter);

#if PRINT_FULL_AUDIO_CHANNELS
		for (int i = 0; i < mixerChannels; i++)
		{
			const Channel& channel = channels[i];
			DebugPrint(Log, "Audio name: %s", channel.audio()->name().c_str());
		}
#endif // PRINT_FULL_AUDIO_CHANNELS
	}
#endif // DEBUG_MODE

}


bool SoundController::isPlaying(const Audio* audio, uintptr_t id) const
{
	for (int i = 0; i < mixerChannels; i++)
	{
		if (channels[i].isPlaying(audio, id))
			return true;
	}

	return false;
}


bool SoundController::hasActiveAudio(const Audio* audio, uintptr_t id) const
{
	for (int i = 0; i < mixerChannels; i++)
	{
		if (channels[i].has(audio, id))
			return true;
	}

	return false;
}


void SoundController::play(const Audio* audio, uintptr_t id, VectorF position)
{
	// Check if the source is already playing this sound
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.isPlaying(audio, id))
		{
			channel.play();
			channel.mSource = position;
			return;
		}
	}

	// Find free channel
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.mState == Channel::Free)
		{
			channel.setAudio(audio);
			channel.mID = id;
			channel.mSource = position;
			channel.play();
			return;
		}
	}

	DebugPrint(Warning, " -------------- No available channels to play audio --------------");
}


void SoundController::loop(const Audio* audio, uintptr_t id, VectorF position)
{
	// Check if the source is already playing this sound
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.isPlaying(audio, id))
		{
			channel.mSource = position;
			return;
		}
	}

	// Find free channel
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.mState == Channel::Free)
		{
			channel.setAudio(audio);
			channel.mID = id;
			channel.mSource = position;
			channel.loop();
			return;
		}
	}

	DebugPrint(Warning, "No available channels to loop audio");
}


void SoundController::pauseSound(const Audio* audio, uintptr_t id)
{
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.isPlaying(audio, id))
		{
			channel.pause();
			return;
		}
	}

	DebugPrint(Warning, "Could not pause audio");
}


void SoundController::resumeSound(const Audio* audio, uintptr_t id)
{
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.isPaused() && channel.has(audio, id))
		{
			channel.resume();
			return;
		}
	}

	DebugPrint(Warning, "Could not resume audio");
}


void SoundController::stopSound(const Audio* audio, uintptr_t id)
{
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.has(audio, id))
		{
			channel.stop();
			return;
		}
	}

	/*DebugPrint*/(Warning, "Could not stop audio");
}


void SoundController::fadeOut(const Audio* audio, uintptr_t id, int ms)
{
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.isPlaying(audio, id))
		{
			channel.fadeOut(ms);
			return;
		}
	}

	DebugPrint(Warning, "Could not fade out audio");
}

void SoundController::fadeIn(const Audio* audio, uintptr_t id, int ms, VectorF position)
{
	// Check if the source is already playing this sound
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.isPlaying(audio, id))
		{
			channel.fadeIn(ms);
			channel.mSource = position;
			return;
		}
	}

	// Find free channel
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.mState == Channel::Free)
		{
			channel.setAudio(audio);
			channel.mID = id;
			channel.mSource = position;
			channel.fadeIn(ms);
			return;
		}
	}

	DebugPrint(Warning, "Could not fade in audio");
}


void SoundController::fadeInMusic(const Audio* audio, uintptr_t id, int ms)
{
	// Check if the music is already playing
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.mState == Channel::Playing)
		{
			// If music is already playing we have to stop and free that first, before we fade in another.
			// We can't have two playing at a time as the faded out music thinks its still playing as there's only 1 global
			// music state and if we play another before the previous one fades out it'll think its always playing and never close
			if (channel.type() == AudioType::Music)
			{
				channel.stop();
				channel.free();
				DebugPrint(Log, "Stopped channel %d music before fading in new music");

				// May as well use the same channel to play the next music file
				channel.setAudio(audio);
				channel.mID = id;
				channel.mSource = VectorF(-1.0f, -1.0f);
				channel.fadeIn(ms);
				return;
			}
		}
	}

	// Find free channel
	for (int i = 0; i < mixerChannels; i++)
	{
		Channel& channel = channels[i];
		if (channel.mState == Channel::Free)
		{
			channel.setAudio(audio);
			channel.mID = id;
			channel.mSource = VectorF(-1.0f, -1.0f);
			channel.fadeIn(ms);
			return;
		}
	}

	DebugPrint(Warning, "Could not fade in audio");
}


void SoundController::setSoundVolume(float volume)
{ 
#if MUTE_AUDIO
	volume = 0;
#endif
	soundVolume = volume;
}


void SoundController::setMusicVolume(float volume)
{
#if MUTE_AUDIO
	volume = 0;
#endif
	musicVolume = volume <= 0.05 ? 0 : volume;
	float mixVolume = maxVolume * musicVolume;
	Mix_VolumeMusic((int)mixVolume);
}


// -- Private Functions -- //

void SoundController::updateMixerVolume(Channel& channel)
{
	float relativeVolume = soundVolume * channel.mVolume * maxVolume;
	Mix_Volume(channel.index(), (int)relativeVolume);
}

float SoundController::attenuation(Channel& channel)
{
	float audioDistance = 0; //distanceSquared(mListener->position(), channel.mSource);
	float attenuation = (mAttenuationDistance - audioDistance) / mAttenuationDistance;
	return Maths::clamp(attenuation, 0.0f, 1.0f);
}