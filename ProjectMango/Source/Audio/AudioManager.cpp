#include "pch.h"
#include "AudioManager.h"
#include "Audio.h"
#include "Game/Data/LoadingManager.h"

AudioManager* AudioManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.audioManager != nullptr, "Audio manager has no been set up yet");
	return gd.audioManager;
}

SoundController* AudioManager::GetController()
{
	return &Get()->mSoundController;
}

AudioManager::AudioManager()
{
	DebugPrint(Log, "Audio manager created");
}

AudioManager::~AudioManager()
{
	FreeAllSoundEffects();

	for( auto iter = musicTracks.begin(); iter != musicTracks.end(); iter++ )
	{
		Mix_FreeMusic(iter->second);
	}	
	musicTracks.clear();

	DebugPrint(Log, "Music Freed");

	DebugPrint(Log, "Audio manager destroyed");
}

//void AudioManager::setSource(Actor* listener, float attenuationDistance)
//{
//	mSoundController.setListener(listener);
//	mSoundController.attenuationDistance(attenuationDistance);
//}

// -- Audio Loading -- //
void AudioManager::preLoad()
{
	// Loading bar music
	//const char* fileName = "Menu";
	//BasicString path = FileManager::Get()->findFile(FileManager::Audio_Music, fileName);

	//Audio *audio = new Music;
	//loadAudio(audio, fileName, path.c_str());
}


void AudioManager::load()
{
	DebugPrint(Log, "\n--- Loading Audio ---");

	//DebugPrint(Log, "Loading Music");
	//loadAllMusic(FileManager::Audio_Music);

	DebugPrint(Log, "\nLoading Sound Effects");
	LoadAllSoundEffects();

	//DebugPrint(Log, "\nSound Groups");
	//fails += loadAllSoundGroups(FileManager::Audio_SoundGroups);

	DebugPrint(Log, "\n--- Audio Loading Complete ---");
}

//
//void AudioManager::unload()
//{
//	mSoundController.ClearChannels();
//	
//	//for( auto iter = musicTracks.begin(); iter != musicTracks.end(); iter++ )
//	//{
//	//	delete iter->second;
//	//}
//
//	//musicTracks.clear();
//
//	FreeAllSoundEffects();
//	DebugPrint(Log, "Audio manager unloaded");
//}


void AudioManager::FreeAllSoundEffects()
{
	for( auto iter = soundEffects.begin(); iter != soundEffects.end(); iter++ )
	{
		Mix_FreeChunk(iter->second);
	}	
	soundEffects.clear();

	DebugPrint(Log, "Sound Effects Freed");
}


void AudioManager::PlaySoundEffect(const char* id, int time)
{
	if(Mix_Chunk* sound_effect = GetSoundEffect(id))
		mSoundController.Play(sound_effect, time, id);
}

void AudioManager::PlayMusic(const char* id)
{
	if(Mix_Music* music = GetMusicTrack(id))
		Mix_PlayMusic(music, -1);
}

//void AudioManager::Update()
//{
//	//mSoundController.Update();
//}

//void AudioManager::push(AudioEvent event)
//{
//	const Audio* audio = getAudio(event.label.c_str());		
//	if (!audio)
//		return;
//
//	switch (event.action)
//	{
//	case AudioEvent::Play:
//		mSoundController.play(audio, event.id, event.source);
//		break;
//	case AudioEvent::Loop:
//		mSoundController.loop(audio, event.id, event.source);
//		break;
//	case AudioEvent::Pause:
//		mSoundController.pauseSound(audio, event.id);
//		break;
//	case AudioEvent::Resume:
//		mSoundController.resumeSound(audio, event.id);
//		break;
//	case AudioEvent::Stop:
//		mSoundController.stopSound(audio, event.id);
//		break;
//	case AudioEvent::FadeIn:
//		mSoundController.fadeIn(audio, event.id, event.time, event.source);
//		break;
//	case AudioEvent::FadeInMusic:
//		mSoundController.fadeInMusic(audio, event.id, event.time);
//		break;
//	case AudioEvent::FadeOut:
//		mSoundController.fadeOut(audio, event.id, event.time);
//		break;
//	default:
//		break;
//	}
//
//
//
//	//if (audio)
//	//{
//	//	event.audio = audio;
//	//	mEvents.push(event);
//	//}
//}

Mix_Chunk* AudioManager::GetSoundEffectNoError(const char* id) const
{
	auto search = soundEffects.find(id);
	if (search != soundEffects.end())
	{
		return search->second;
	}

	return nullptr;
}
Mix_Chunk* AudioManager::GetSoundEffect(const char* id) const
{
	auto search = soundEffects.find(id);

	if (search != soundEffects.end())
	{
		return search->second;
	}

	DebugPrint(Warning, "No item in audio map with label: %s", id);
	return nullptr;
}

Mix_Music* AudioManager::GetMusicTrack(const char* id)
{
	auto search = musicTracks.find(id);
	if (search != musicTracks.end())
	{
		return search->second;
	}
	else
	{
		const FileManager* fm = FileManager::Get();
		BasicString file;
		fm->FindFile(FileManager::Audio_Music, id, file);
		return LoadMusicTrack(id, file.c_str());
	}
}

const char* AudioManager::GetSoundEffectId(const Mix_Chunk* sound_effect) const
{
	std::unordered_map<StringBuffer64, Audio*>::const_iterator iter;
	for ( auto iter = soundEffects.begin(); iter != soundEffects.end(); iter++)
	{
		if (iter->second == sound_effect)
		{
			return iter->first.c_str();
		}
	}

	DebugPrint(Warning, "The audio file was not found in the audio bank");
	return nullptr;
}


// -- Audio Control -- //
//void AudioManager::toggleMute()
//{
//	DebugPrint(Warning, "UNIMPLEMENTED");
//	//mSoundController.toggleMute();
//}
//
//void AudioManager::mute(bool shouldMute)
//{
//	DebugPrint(Warning, "UNIMPLEMENTED");
//	//mSoundController.mute(shouldMute);
//}


//void AudioManager::setSoundVolume(float volume)
//{
//	mSoundController.setSoundVolume(volume);
//}
//
//float AudioManager::soundVolume() const
//{
//	return mSoundController.getSoundVolume();
//}
//
//void AudioManager::setMusicVolume(float volume)
//{
//	mSoundController.setMusicVolume(volume);
//}
//
//float AudioManager::musicVolume() const
//{
//	return mSoundController.getMusicVolume();
//}

//bool AudioManager::isPlaying(const char* label, const void* sourceId) const
//{
//	Audio* audio = getAudio(label);
//
//	if (audio)
//		return mSoundController.isPlaying(audio, reinterpret_cast<uintptr_t>(sourceId));
//	else
//	{
//		DebugPrint(Warning, "No audio with label '%s' found, cannot check if playing", label);
//		return false;
//	}
//}
//
//bool AudioManager::isActive(const char* label, const void* sourceId) const
//{
//	Audio* audio = getAudio(label);
//
//	if (audio)
//		return mSoundController.hasActiveAudio(audio, reinterpret_cast<uintptr_t>(sourceId));
//	else
//	{
//		DebugPrint(Warning, "No audio with label '%s' found, cannot check if active", label);
//		return false;
//	}
//}

// --- Private Functions --- //
//int AudioManager::loadAllMusic(FileManager::Folder folder)
//{
//	const FileManager* fm = FileManager::Get();
//
//	int fails = 0;
//	std::vector<BasicString> paths;
//	fm->GetFilesInFolder(folder, paths);
//
//	for (const BasicString& path : paths)
//	{
//		StringBuffer64 audio_name = fm->getItemName(path.c_str());
//		// Dont double load pre loaded music
//		if (mAudioBank.count(audio_name) == 0)
//		{
//			Audio *audio = new Music;
//			fails += !loadAudio(audio, audio_name.c_str(), path.c_str());
//		}
//	}
//
//	return fails;
//}

int AudioManager::LoadAllSoundEffects()
{
	if(soundEffects.size() > 0)
	{
		FreeAllSoundEffects();
	}

	const FileManager* fm = FileManager::Get();

	int fails = 0;
	std::vector<BasicString> paths;
	fm->GetFilesInFolder(FileManager::Audio_Sound, paths);

	for (const BasicString& path : paths)
	{
		StringBuffer64 audio_name = fm->getItemName(path.c_str());

		fails += !LoadSoundEffect(audio_name.c_str(), path.c_str());
	}

	return fails;
}

bool AudioManager::LoadSoundEffect(const char* name, const char* filePath)
{
	if(Mix_Chunk* sound = Mix_LoadWAV(filePath))
	{
		// Add to has loaded files
		if (LoadingManager* lm = LoadingManager::Get())
			lm->successfullyLoaded(filePath);

		soundEffects[name] = sound;
		DebugPrint(Log, "Successfully loaded sound effect '%s'", name, filePath);
		return true;
	}
	else
	{
		DebugPrint(Warning, "Failed to load sound effect: %s(%s). SDL_mixer Error: %s", name, filePath, Mix_GetError());
		return false;
	}
}

Mix_Music* AudioManager::LoadMusicTrack(const char* name, const char* filePath)
{
	if(Mix_Music* music = Mix_LoadMUS(filePath))
	{
		// Add to has loaded files
		if (LoadingManager* lm = LoadingManager::Get())
			lm->successfullyLoaded(filePath);

		musicTracks[name] = music;
		DebugPrint(Log, "Successfully loaded music track '%s'", name, filePath);
		return music;
	}
	else
	{
		DebugPrint(Warning, "Failed to load music track: %s(%s). SDL_mixer Error: %s", name, filePath, Mix_GetError());
		return nullptr;
	}
}

//int AudioManager::loadAllSoundGroups(FileManager::Folder folder)
//{
//	int fails = 0;
//	std::vector<BasicString> folderPaths = FileManager::Get()->foldersInFolder(folder);
//	const FileManager* fm = FileManager::Get();
//
//	for (const BasicString& folderPath : folderPaths)
//	{
//		StringBuffer64 audio_name = fm->getItemName(folderPath.c_str());
//		// Dont double load pre loaded music
//		if (mAudioBank.count(audio_name) == 0)
//		{
//			Audio* audio = new SoundGroup;
//			fails += !loadAudio(audio, audio_name.c_str(), folderPath.c_str());
//		}
//	}
//
//	return fails;
//}
