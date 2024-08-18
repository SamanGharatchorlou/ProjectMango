#include "pch.h"
#include "UIManager.h"

#include "Input/InputManager.h"
#include "Graphics/TextureManager.h"
#include "Core/Helpers.h"

#include "UIEventHandler.h"
#include "Screens/ScreenHeaders.h"
#include "Game/SystemStateManager.h"
#include "System/Files/ConfigManager.h"
#include "System/Files/Config.h"

UIManager::UIManager() : mCursor(nullptr)
{ 
	memset(mScreens, 0, sizeof(Screen*) * UIScreen::Type::Count);
}

UIManager::~UIManager()
{
	clearScreens();
}


void UIManager::init()
{
	//mController.init(gameController, this);
#if UI_EDITOR
	mEditor.init(&mController);
#endif

}


void UIManager::preLoad()
{
	LoadingScreen* loading_screen = new LoadingScreen;
	mScreens[UIScreen::Type::LoadingScreen] = loading_screen;

	ScreenBuilder::populateScreen(*loading_screen);
}

void UIManager::load()
{
	mScreens[UIScreen::Type::Game] = new GameScreen;
	mScreens[UIScreen::Type::GameOver] = new GameOverScreen;
	mScreens[UIScreen::Type::Pause] = new PauseScreen;
	mScreens[UIScreen::Type::Settings] = new SettingsScreen;
	mScreens[UIScreen::Type::Popup] = new PopupScreen;
	mScreens[UIScreen::Type::MainMenu] = new MainMenuScreen;

	for (int i = 0; i < UIScreen::Type::Count; i++)
	{
		if(mScreens[i])
			ScreenBuilder::populateScreen(*mScreens[i]);
	}

	ConfigManager* cm = ConfigManager::Get();
	FileManager* fm = FileManager::Get();

	//const std::vector<StringBuffer64> popup_configs = fm->fileNamesInFolder(FileManager::Config_Popups);
	//for (u32 i = 0; i < popup_configs.size(); i++)
	//{
	//	const StringBuffer64 name = fm->getItemName(popup_configs[i].c_str());
	//	cm->add<PopupConfig>(name.c_str());
	//}
}


Screen* UIManager::getScreen(UIScreen::Type type)
{
	return mScreens[type];
}


void UIManager::clearScreens()
{
	mController.clearScreenStack();

	for (Uint32 i = 0; i < UIScreen::Type::Count; i++)
	{
		delete mScreens[i];
	}
	memset(mScreens, 0, sizeof(Screen*)* UIScreen::Type::Count);
}


void UIManager::handleInput(InputManager* input)
{
#if UI_EDITOR
	mEditor.handleInput(input);
#endif

	node<Screen*>* screen_node = mController.mScreens.getActiveStateNode();

	// update the below one first (order doesnt really matter here)
	if (HasFlag(screen_node->data->mFlags, Screen::HanleBelowInputs))
	{
		screen_node->prev->data->handleInput(input);
		screen_node->prev->data->updateInputs(input);
	}

	screen_node->data->handleInput(input);
	screen_node->data->updateInputs(input);
}


void UIManager::update() 
{
	node<Screen*>* screen_node = mController.mScreens.getActiveStateNode();

	// update the below one first (order doesnt really matter here)
	if (HasFlag(screen_node->data->mFlags, Screen::UpdateBelow))
	{
		screen_node->prev->data->Update();
	}

	screen_node->data->Update();
}


Screen* UIManager::getActiveScreen()
{
	return mController.getActiveScreen();
}


void UIManager::render()
{
	if (mController.persistingScreen())
		mController.persistingScreen()->render();

	node<Screen*>* screen_node = mController.mScreens.getActiveStateNode();

	// render the below one first
	if (HasFlag(screen_node->data->mFlags, Screen::RenderBelow))
	{
		screen_node->prev->data->render();
	}

	screen_node->data->render();

#if UI_EDITOR
	mEditor.render();
#endif
		
	mCursor->render();
}


void UIManager::handleEvent(EventData& data)
{
	UIEventHandler eventHandler;
	eventHandler.handleEvent(data);
}


void UIManager::initCursor(Cursor* cursor)
{
	mCursor = cursor;
}


void UIManager::setCursorTexture(Texture* texture) 
{ 
	const StringBuffer64& textureName = TextureManager::Get()->getTextureName(texture);
	
	// Set cursor mode
	if (textureName == "GameCursor")
		mCursor->setMode(Cursor::Game_Red);
	else if (textureName == "GameCursorGreen")
		mCursor->setMode(Cursor::Game_Green);
	else if (textureName == "UICursor")
		mCursor->setMode(Cursor::Menu);
	else
		DebugPrint(Warning, "No cursor mode has been linked to texture %s", textureName.c_str());

	// Texture offset, the cursor point is in different places...
	if (textureName == "GameCursor")
		mCursor->setCursorOffsetPoint(VectorF());
	else
		mCursor->setCursorOffsetPoint(mCursor->size() / -2.0f);

	mCursor->setTexture(texture);
}
