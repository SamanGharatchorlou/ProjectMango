#pragma once

#include "Game/States/StateMachine.h"
#include "Screens/Screen.h"
#include "ScreenTypes.h"

// Couples the UI to the Game
class ScreenController
{
	friend UIManager;

public:
	ScreenController();

	void openPopup(const char* textFile);
	
	void addScreen(UIScreen::Type type, u32 flags = 0);
	void replaceScreen(UIScreen::Type type);
	void popScreen();

	u32 screenCount() const { return (u32)mScreens.size(); }
	void clearScreenStack();

	Screen* getActiveScreen() const { return &mScreens.getActiveState(); }

	// TODO: make const(), need render() to be const first
	void setPersistingScreen(Screen* screen) { mPersistingScreen = screen; }
	Screen* persistingScreen() const { return mPersistingScreen; }

	void enablePopups(bool enable) { mEnablePopups = enable; }


private:
	
	StateMachine<Screen> mScreens;
	Screen* mPersistingScreen;

	bool mEnablePopups;
};