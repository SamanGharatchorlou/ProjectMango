#include "pch.h"
#include "AudioManager.h"
#include "Audio.h"
#include "Game/Data/GameData.h"

#include "Game/Data/LoadingManager.h"



AudioManager* AudioManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.audioManager != nullptr, "Audio manager has no been set up yet");
	return gd.audioManager;
}



AudioManager::AudioManager()
{
	DebugPrint(Log, "Audio manager created");
}


AudioManager::~AudioManager()
{
	DebugPrint(Log, "Audio manager destroyed");
}

void AudioManager::init()
{
	mSoundController.init();
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
	const char* fileName = "Menu";
	BasicString path = FileManager::Get()->findFile(FileManager::Audio_Music, fileName);

	Audio *audio = new Music;
	loadAudio(audio, fileName, path.c_str());
}


void AudioManager::load()
{
	DebugPrint(Log, "\n--- Loading Audio ---");
	int fails = 0;

	DebugPrint(Log, "\nBackground Music");
	fails += loadAllMusic(FileManager::Audio_Music);

	DebugPrint(Log, "\nAudio Effects");
	fails += loadAllSound(FileManager::Audio_Sound);

	DebugPrint(Log, "\nSound Groups");
	fails += loadAllSoundGroups(FileManager::Audio_SoundGroups);

	DebugPrint(Log, "\n--- Audio Loading Complete: %d Failures ---", fails);
}


void AudioManager::unload()
{
	mSoundController.clear();

	for (std::pair<StringBuffer32, Audio*> audio : mAudioBank)
	{
		delete audio.second;
	}

	mAudioBank.clear();

	DebugPrint(Log, "Audio manager unloaded");
}

void AudioManager::Update()
{
	mSoundController.Update();
}

void AudioManager::push(AudioEvent event)
{
	const Audio* audio = getAudio(event.label.c_str());		
	if (!audio)
		return;

	switch (event.action)
	{
	case AudioEvent::Play:
		mSoundController.play(audio, event.id, event.source);
		break;
	case AudioEvent::Loop:
		mSoundController.loop(audio, event.id, event.source);
		break;
	case AudioEvent::Pause:
		mSoundController.pauseSound(audio, event.id);
		break;
	case AudioEvent::Resume:
		mSoundController.resumeSound(audio, event.id);
		break;
	case AudioEvent::Stop:
		mSoundController.stopSound(audio, event.id);
		break;
	case AudioEvent::FadeIn:
		mSoundController.fadeIn(audio, event.id, event.time, event.source);
		break;
	case AudioEvent::FadeInMusic:
		mSoundController.fadeInMusic(audio, event.id, event.time);
		break;
	case AudioEvent::FadeOut:
		mSoundController.fadeOut(audio, event.id, event.time);
		break;
	default:
		break;
	}



	//if (audio)
	//{
	//	event.audio = audio;
	//	mEvents.push(event);
	//}
}


Audio* AudioManager::getAudio(const char* label) const
{
	auto search = mAudioBank.find(label);

	if (search != mAudioBank.end())
	{
		return search->second;
	}

	DebugPrint(Warning, "No item in audio map with label: %s", label);
	return nullptr;
}


// -- Audio Control -- //
void AudioManager::toggleMute()
{
	DebugPrint(Warning, "UNIMPLEMENTED");
	//mSoundController.toggleMute();
}

void AudioManager::mute(bool shouldMute)
{
	DebugPrint(Warning, "UNIMPLEMENTED");
	//mSoundController.mute(shouldMute);
}


void AudioManager::setSoundVolume(float volume)
{
	mSoundController.setSoundVolume(volume);
}

float AudioManager::soundVolume() const
{
	return mSoundController.getSoundVolume();
}

void AudioManager::setMusicVolume(float volume)
{
	mSoundController.setMusicVolume(volume);
}

float AudioManager::musicVolume() const
{
	return mSoundController.getMusicVolume();
}

bool AudioManager::isPlaying(const char* label, const void* sourceId) const
{
	Audio* audio = getAudio(label);

	if (audio)
		return mSoundController.isPlaying(audio, reinterpret_cast<uintptr_t>(sourceId));
	else
	{
		DebugPrint(Warning, "No audio with label '%s' found, cannot check if playing", label);
		return false;
	}
}

bool AudioManager::isActive(const char* label, const void* sourceId) const
{
	Audio* audio = getAudio(label);

	if (audio)
		return mSoundController.hasActiveAudio(audio, reinterpret_cast<uintptr_t>(sourceId));
	else
	{
		DebugPrint(Warning, "No audio with label '%s' found, cannot check if active", label);
		return false;
	}
}

// --- Private Functions --- //
int AudioManager::loadAllMusic(FileManager::Folder folder)
{
	int fails = 0;
	std::vector<BasicString> paths = FileManager::Get()->allFilesInFolder(folder);
	const FileManager* fm = FileManager::Get();

	for (const BasicString& path : paths)
	{
		StringBuffer32 audio_name = fm->getItemName(path.c_str());
		// Dont double load pre loaded music
		if (mAudioBank.count(audio_name) == 0)
		{
			Audio *audio = new Music;
			fails += !loadAudio(audio, audio_name.c_str(), path.c_str());
		}
	}

	return fails;
}


int AudioManager::loadAllSound(FileManager::Folder folder)
{
	int fails = 0;
	std::vector<BasicString> paths = FileManager::Get()->allFilesInFolder(folder);
	const FileManager* fm = FileManager::Get();

	for (const BasicString& path : paths)
	{
		StringBuffer32 audio_name = fm->getItemName(path.c_str());
		// Dont double load pre loaded music
		if (mAudioBank.count(audio_name) == 0)
		{
			Audio* audio = new Sound;
			fails += !loadAudio(audio, audio_name.c_str(), path.c_str());
		}
	}

	return fails;
}


int AudioManager::loadAllSoundGroups(FileManager::Folder folder)
{
	int fails = 0;
	std::vector<BasicString> folderPaths = FileManager::Get()->foldersInFolder(folder);
	const FileManager* fm = FileManager::Get();

	for (const BasicString& folderPath : folderPaths)
	{
		StringBuffer32 audio_name = fm->getItemName(folderPath.c_str());
		// Dont double load pre loaded music
		if (mAudioBank.count(audio_name) == 0)
		{
			Audio* audio = new SoundGroup;
			fails += !loadAudio(audio, audio_name.c_str(), folderPath.c_str());
		}
	}

	return fails;
}


bool AudioManager::loadAudio(Audio* audio, const char* name, const char* filePath)
{
	bool success = true;
	if (audio->load(filePath))
	{
		// Add to has loaded files
		if (LoadingManager* lm = LoadingManager::Get())
			lm->successfullyLoaded(filePath);

		mAudioBank[name] = audio;
		DebugPrint(Log, "Successfully loaded audio '%s'", name);
	}
	else
	{
		DebugPrint(Warning, "Failure: audio NOT loaded '%s' at %s\n. SDL_mixer Error: %s", name, filePath, Mix_GetError());
		success = false;
	}

	return success;
}


const char* AudioManager::getLabel(Audio* audio) const
{
	std::unordered_map<StringBuffer32, Audio*>::const_iterator iter;
	for (iter = mAudioBank.begin(); iter != mAudioBank.end(); iter++)
	{
		if (audio == iter->second)
		{
			return iter->first.c_str();
		}
	}

	DebugPrint(Warning, "The audio file was not found in the audio bank");
	return nullptr;
}