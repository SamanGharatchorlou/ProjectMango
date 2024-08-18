#include "pch.h"
#include "ScreenController.h"

#include "UI/UIManager.h"
#include "Screens/PopupScreen.h"
#include "Core/Helpers.h"

#include "Game/SystemStateManager.h"
#include "Audio/AudioManager.h"
#include "System/Files/ConfigManager.h"

ScreenController::ScreenController() : mScreens(new NullScreen, false) 
{
	GameSettingsConfig* config = ConfigManager::Get()->getConfig<GameSettingsConfig>("GameSettings");
	mEnablePopups = config->settings.getBool("Popups");
}

void ScreenController::clearScreenStack()
{
	// Leave the base null state
	while (screenCount() > 1)
	{
		popScreen();
	}

	mPersistingScreen = nullptr;
}

void ScreenController::addScreen(UIScreen::Type type, u32 flags)
{
	Screen* screen = GameData::Get().uiManager->getScreen(type);
	screen->mFlags = flags;

	mScreens.addState(screen);


#if !DISABLE_UI_AUDIO
	AudioManager::Get()->push(AudioEvent(AudioEvent::Play, "CloseMenu", nullptr));
#endif
}

void ScreenController::replaceScreen(UIScreen::Type type)
{
#if DEBUG_MODE
	if (mScreens.size() <= 1)
	{
		StringBuffer32 screenName;
		type >> screenName;
		DebugPrint(Warning, "Cannot replace the Null state with state %s, maybe add instead?", screenName.c_str());
		return;
	}
#endif

	Screen* screen = GameData::Get().uiManager->getScreen(type);
	mScreens.replaceState(screen);
}


void ScreenController::popScreen()
{
	mScreens.popState();
#if !DISABLE_UI_AUDIO
	AudioManager::Get()->push(AudioEvent(AudioEvent::Play, "CloseMenu", nullptr));
#endif
}


void ScreenController::openPopup(const char* popupInfo)
{
	if (!mEnablePopups)
		return;

	if (PopupScreen* popup = GameData::Get().uiManager->getScreen<PopupScreen>(UIScreen::Type::Popup))
	{
		PopupConfig* config = ConfigManager::Get()->getConfig<PopupConfig>(popupInfo);
		popup->build(config->title.c_str(), config->body.c_str());

		u32 flags = 0;
		SetFlag<u32>(flags, Screen::RenderBelow | Screen::HanleBelowInputs);

		addScreen(UIScreen::Type::Popup, flags);
		setPersistingScreen(popup);
		//GameData::Get().systemStateManager->addState(SystemStates::PauseState);
	}
	else
	{
		DebugPrint(Warning, "Popup screen already in use, cannot make another");
	}
}
