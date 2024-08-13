#include "pch.h"
#include "SettingsScreen.h"

#include "UI/ScreenController.h"
#include "Audio/AudioManager.h"
#include "UI/UIManager.h"


void SettingsScreen::Init()
{
	AudioManager* audio = AudioManager::Get();

	float musicVolume = audio->musicVolume();
	getSlider("MusicSlider")->setSliderValue(musicVolume);

	float soundVolume = audio->soundVolume();
	getSlider("SoundSlider")->setSliderValue(soundVolume);
}


void SettingsScreen::Update()
{
	AudioManager* audio = AudioManager::Get();

	UISlider* soundSlider = getSlider("SoundSlider");
	if (soundSlider->isActive())
	{
		audio->setSoundVolume(soundSlider->getValue());
	}

	UISlider* musicSlider = getSlider("MusicSlider");
	if (musicSlider->isActive())
	{
		audio->setMusicVolume(musicSlider->getValue());
	}

	if (released("CloseButton") || released("BackButton"))
	{
		GameData::Get().uiManager->controller()->popScreen();
	}
}