#include "pch.h"
#include "Audio.h"

/*

//bool Sound::load(const char* filePath)
//{
//	mChunk = Mix_LoadWAV(filePath);
//
//	if (!mChunk)
//	{
//		DebugPrint(Warning, "Failed to load sound audio file %s. SDL_mixer Error: %s", filePath, Mix_GetError());
//		return false;
//	}
//	else
//	{
//#if AUDIO_LOGGING
//		mFilePath = filePath;
//#endif
//		return true;
//	}
//}
//Sound::~Sound()
//{
//	Mix_FreeChunk(chunk);
//}

// --- Sound --- //
void Sound::Play(int channel)
{
	Mix_PlayChannel(channel, chunk, loops);
}

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

void Sound::FadeIn(int channel, int ms)
{
	Mix_FadeInChannel(channel, chunk, loops, ms);
}

void Sound::FadeOut(int channel, int ms)
{
	Mix_FadeOutChannel(channel, ms);
}



//Music::~Music()
//{
//	Mix_FreeMusic(mMusic);
//}


//bool Music::load(const char* filePath)
//{
//	mMusic = Mix_LoadMUS(filePath);
//
//	if (!mMusic)
//	{
//		DebugPrint(Warning, "Failed to load music audio file %s. SDL_mixer Error: %s", filePath, Mix_GetError());
//		return false;
//	}
//	else
//	{
//#if AUDIO_LOGGING
//		mFilePath = filePath;
//#endif
//		return true;
//	}
//}


// --- Music --- //
void Music::Play()
{
	Mix_PlayMusic(music, loops);

#if PRINT_PLAY_AUDIO
	DebugPrint(Log, "Playing music %s", mFilePath.c_str());
#endif
}

void Music::Resume()
{
	Mix_ResumeMusic();
}


void Music::Pause()
{
	Mix_PauseMusic();
}


void Music::Stop()
{
	Mix_HaltMusic();
}

bool Music::IsPlaying()
{
	return Mix_PlayingMusic();
}

void Music::FadeIn(int ms)
{
	Mix_FadeInMusic(music, loops, ms);
}

void Music::FadeOut(int ms)
{
	Mix_FadeOutMusic(ms);
}



//// --- Audio Group --- //
//SoundGroup::~SoundGroup()
//{
//	for (int i = 0; i < group.size(); i++)
//	{
//		Audio* audio = group[i];
//		delete audio;
//	}
//}
//
//bool SoundGroup::load(const char* directoryPath)
//{
//	StringBuffer64 groupName = FileManager::Get()->getItemName( directoryPath);
//	std::vector<BasicString> audioFilePaths = FileManager::Get()->fullPathsInFolder(directoryPath);
//
//	for (int i = 0; i < audioFilePaths.size(); i++)
//	{
//		Audio* audio = new Sound;
//
//		if (audio->load(audioFilePaths[i].c_str()))
//		{
//			group.push_back(audio);
//		}
//		else
//		{
//			DebugPrint(Warning, "Failure: sound '%s' NOT loaded into group '%s' in directory '%s'\n. SDL_mixer Error: %s",
//				FileManager::Get()->getItemName(audioFilePaths[i].c_str()).c_str(), groupName.c_str(), directoryPath, Mix_GetError());
//		}
//	}
//
//#if AUDIO_LOGGING
//	if(group.size() > 0)
//		mFilePath = audioFilePaths[0];
//#endif
//
//	DebugPrint(Log, "Successfully loaded %d/%d audio files into group '%s'", group.size(), audioFilePaths.size(), groupName.c_str());
//	return (bool)group.size();
//}
//
//
//void SoundGroup::play(int channel) const
//{
//	int randomNumber = Maths::randomNumberBetween(0, (u32)group.size());
//	group[randomNumber]->play(channel);
//}
//
//
//void SoundGroup::playNext(int channel)
//{
//	playingIndex = playingIndex + 1 >= group.size() ? 0 : playingIndex + 1;
//	group[playingIndex]->play(channel);
//}
//
//void SoundGroup::stop(int channel)
//{
//	playingIndex = 0;
//	Mix_HaltChannel(channel);
//}
//
//bool SoundGroup::isPlaying(int channel) const
//{
//	return group[playingIndex]->isPlaying(channel);
//}
//
//void SoundGroup::pause(int channel) const
//{
//	group[playingIndex]->pause(channel);
//}
//
//void SoundGroup::resume(int channel) const
//{
//	group[playingIndex]->resume(channel);
//}
//
//void SoundGroup::fadeIn(int channel, int ms) const
//{
//	int randomNumber = Maths::randomNumberBetween(0, (u32)group.size());
//	group[randomNumber]->fadeIn(channel, ms);
//}
//
//void SoundGroup::fadeOut(int channel, int ms) const
//{
//	group[playingIndex]->fadeOut(channel, ms);
//}
*/