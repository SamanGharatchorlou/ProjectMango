#include "pch.h"
#include "SoundController.h"

#include "Audio.h"

// Audio
#define MUTE_AUDIO 0
#define DISABLE_UI_AUDIO 0
#define PRINT_PLAYING_AUDIO 0
#define PRINT_FULL_AUDIO_CHANNELS 0
#define AUDIO_LOGGING (PRINT_PLAYING_AUDIO || PRINT_FULL_AUDIO_CHANNELS)

// global limit - the max number of the same audio that can play at a time
static constexpr int c_audioPlayCountLimit = 3;

SoundController::SoundController() : soundVolume(1.0f), musicVolume(1.0f)
{
	Mix_AllocateChannels(c_mixerChannels);
	ClearChannels();
}


void SoundController::ClearChannels()
{
	for (int i = 0; i < c_mixerChannels; i++)
	{
		Mix_HaltChannel(i);
		channels[i] = Channel();
	}
}

/*
void SoundController::Update()
{
#if DEBUG_MODE
	int counter = 0;
#endif
	float vol = getMusicVolume();
	int vim = Mix_VolumeMusic(-1);

	for (int i = 0; i < c_mixerChannels; i++)
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
	if (counter >= c_mixerChannels)
	{
		int playingCounter = 0;
		int pauseCounter = 0;

		for (int i = 0; i < c_mixerChannels; i++)
		{
			const Channel& channel = channels[i];
			if (channel.hasPlayingState())
				playingCounter++;
			else if (channel.isPaused())
				pauseCounter++;
		}

		DebugPrint(Warning, "All the the 8 audio channels are being used. %d channels are playing and %d channels are paused", playingCounter, pauseCounter);

#if PRINT_FULL_AUDIO_CHANNELS
		for (int i = 0; i < c_mixerChannels; i++)
		{
			const Channel& channel = channels[i];
			DebugPrint(Log, "Audio name: %s", channel.audio()->name().c_str());
		}
#endif // PRINT_FULL_AUDIO_CHANNELS
	}
#endif // DEBUG_MODE

}
*/

//bool SoundController::isPlaying(const Audio* audio, uintptr_t id) const
//{
//	for (int i = 0; i < c_mixerChannels; i++)
//	{
//		if (channels[i].isPlaying(audio, id))
//			return true;
//	}
//
//	return false;
//}
//
//
//bool SoundController::hasActiveAudio(const Audio* audio, uintptr_t id) const
//{
//	for (int i = 0; i < c_mixerChannels; i++)
//	{
//		if (channels[i].has(audio, id))
//			return true;
//	}
//
//	return false;
//}


//void Sound::Stop(int channel)
//{
//	Mix_HaltChannel(channel);
//}
//
//bool Sound::IsPlaying(int channel) 
//{
//	return Mix_Playing(channel);
//}
//
//void Sound::Pause(int channel)
//{
//	Mix_Pause(channel);
//}
//
//void Sound::Resume(int channel)
//{
//	Mix_Resume(channel);
//}


bool SoundController::IsPlaying(Mix_Chunk* sound) const
{
	for (int i = 0; i < c_mixerChannels; i++)
	{
		if ( channels[i].sound == sound )
		{
			return Mix_Playing(i);
		}
	}

	return false;
}

bool SoundController::IsPlaying(Channel channel) const
{
	return Mix_Playing(channel.index);
}

bool SoundController::Play( Mix_Chunk* sound, int time, const char* id )
{
	int loops = 0;

	// Check if the source is already playing this sound
	int playing_count = 0;

	u32 lowest_tick = UINT32_MAX;
	int best_channel = -1;
	for (int i = 0; i < c_mixerChannels; i++)
	{
		if ( channels[i].sound == sound && Mix_Playing(i) )
		{
			playing_count++;

			// find the channel we played first
			if(channels[i].playedAtTick < lowest_tick)
			{
				best_channel = i;
				lowest_tick = channels[i].playedAtTick;
			}
		}
	}

	// todo: this doesnt work for audio groups since they have different names
	if(playing_count >= c_audioPlayCountLimit && best_channel != -1)
	{
		Mix_HaltChannel(best_channel);
	}

	// Find free channel
	for (int i = 0; i < c_mixerChannels; i++)
	{
		if(!Mix_Playing(i))
		{
			channels[i] = Channel();
			channels[i].index = i;
			channels[i].sound = sound;
			channels[i].playedAtTick = SDL_GetTicks();
			
			Mix_PlayChannelTimed(i, sound, loops, time);
			return true;
		}
	}

	DebugPrint(Warning, "no available channels to play audio");
	return false;
}

bool SoundController::Pause(Mix_Chunk* sound)
{
	// Check if the source is already playing this sound
	for (int i = 0; i < c_mixerChannels; i++)
	{
		if ( channels[i].sound == sound )
		{
			Mix_Pause(i);
			return true;
		}
	}

	return false;
}


bool SoundController::Resume(Mix_Chunk* sound)
{
	for (int i = 0; i < c_mixerChannels; i++)
	{
		if ( channels[i].sound == sound )
		{
			Mix_Resume(i);
			return true;
		}
	}

	return false;
}


bool SoundController::Stop(Mix_Chunk* sound)
{
	for (int i = 0; i < c_mixerChannels; i++)
	{
		if ( channels[i].sound == sound )
		{
			Mix_HaltChannel(i);
			return true;
		}
	}

	return false;
}

/*
void SoundController::fadeOut(const Audio* audio, uintptr_t id, int ms)
{
	for (int i = 0; i < c_mixerChannels; i++)
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
	for (int i = 0; i < c_mixerChannels; i++)
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
	for (int i = 0; i < c_mixerChannels; i++)
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
	for (int i = 0; i < c_mixerChannels; i++)
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
				DebugPrint(Log, "Stopped channel %d music before fading in new music", i);

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
	for (int i = 0; i < c_mixerChannels; i++)
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
*/


void SoundController::SetSoundVolume(float volume)
{ 
#if MUTE_AUDIO
	volume = 0;
#endif

	soundVolume = volume <= 0.05 ? 0 : volume;

	for (int i = 0; i < c_mixerChannels; i++)
	{
		float relativeVolume = soundVolume * channels[i].volume * maxVolume;
		Mix_Volume(i, (int)(relativeVolume + 0.5f));
	}
}


void SoundController::SetMusicVolume(float volume)
{
#if MUTE_AUDIO
	volume = 0;
#endif

	musicVolume = volume <= 0.05 ? 0 : volume;

	float mixVolume = maxVolume * musicVolume;
	Mix_VolumeMusic((int)(mixVolume + 0.5f));
}


// -- Private Functions -- //

//void SoundController::updateMixerVolume(Channel& channel)
//{
//	float relativeVolume = soundVolume * channel.mVolume * maxVolume;
//	Mix_Volume(channel.index(), (int)relativeVolume);
//}

//float SoundController::attenuation(Channel& channel)
//{
//	float audioDistance = 0; //distanceSquared(mListener->position(), channel.mSource);
//	float attenuation = (mAttenuationDistance - audioDistance) / mAttenuationDistance;
//	return Maths::clamp(attenuation, 0.0f, 1.0f);
//}