#pragma once

namespace UIScreen
{
	enum Type : Uint32
	{
		// Pre loading
		LoadingScreen,

		None,
		MainMenu,
		Game,
		GameOver,
		Pause,
		Settings,
		Popup,
		Count
	};


	// Type to string
	inline void operator >>(Type type, StringBuffer32& str)
	{
		if (type == Type::LoadingScreen)
			str = "LoadingScreen";
		else if (type == Type::MainMenu)
			str = "MainMenuScreen";
		else if (type == Type::Game)
			str = "GameScreen";
		else if (type == Type::GameOver)
			str = "GameOverScreen";
		else if (type == Type::Pause)
			str = "PauseScreen";
		else if (type == Type::Settings)
			str = "SettingsScreen";
		else if (type == Type::Popup)
			str = "PopupScreen";
		else
		{
			str.clear();
			DebugPrint(Warning, "No screen type to string conversion defined for screen type %d", type);
		}
	}


	template<class T>
	inline Type operator +(Type a, T b)
	{
		int sum = static_cast<int>(a) + static_cast<int>(b);
		return static_cast<Type>(sum);
	}



	//enum class Item
	//{
	//	None,

	//	// Loading
	//	LoadingBar,

	//	// Character Selection
	//	PlayButton,
	//	Tutorial,
	//	LeftArrow,
	//	RightArrow,

	//	// Game
	//	Health,
	//	Exp,
	//	Armor,

	//	// Pause
	//	Resume,
	//	Settings,
	//	Restart,
	//	Quit,

	//	// Settings
	//	Music,
	//	Sound,
	//	Mute,
	//	Close,
	//	Back,

	//	// Game over
	//	Continue
	//};
}