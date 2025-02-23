#include "pch.h"
#include "GameSetup.h"

#include "Graphics/Renderer.h"
#include "System/Window.h"
#include "Audio/AudioManager.h"
#include "System/Files/ConfigManager.h"

// NSIS Installer
#define TWEAK_OUTPUT_NSIS_FOLDER_INFO 0
#define TWEAK_HIDE_CONSOLE 1

#if TWEAK_OUTPUT_NSIS_FOLDER_INFO
#include "Utilities/NSISFileOutput.h"
#endif

GameSetup::GameSetup()
{
	srand((unsigned int)time(NULL)); // random seed
}

void GameSetup::initFileSystem()
{
	FileManager::Get()->init();

}

void GameSetup::initGameData(GameData& game_data)
{
	game_data.setup();

	ConfigManager* cm = ConfigManager::Get();

	int displays = -1;
	//SDL_DisplayID *displays = SDL_GetDisplays(&num_displays);
	//SDL_Log("Found %d display(s)", num_displays);

	// todo: get window size and resize the window based on that
	// so i dont have to faff around with the screen size
	cm->AddAndLoad("GameSettings", Config::XML);

	Window* window = initSDLWindow();
	game_data.init(window);

	
	
	//// todo not working?
	//SDL_Rect rect;
	//bool success = SDL_GetDisplayBounds(0, &rect);
	//if(!success)
	//	DebugPrint(Error, "%s", SDL_GetError());
	//
	//SDL_Rect rect2;
	//bool success2 = SDL_GetDisplayUsableBounds(0, &rect2);
	//if(!success2)
	//	DebugPrint(Error, "%s", SDL_GetError());

	//Config* gs = cm->GetConfig<Config>("GameSettings");

	// set default audio values from settings
	//AudioManager* audio = AudioManager::Get();
	//audio->setMusicVolume(gs->settings.getFloat("MusicVolume") / 100.0f);
	//audio->setSoundVolume(gs->settings.getFloat("SoundVolume") / 100.0f);
}

Window* GameSetup::initSDLWindow()
{
	HWND windowHandle = GetConsoleWindow();
#if HIDE_CONSOLE
	ShowWindow(windowHandle, SW_HIDE);
#else
	ShowWindow(windowHandle, SW_SHOW);
#endif

#if OUTPUT_NSIS_FOLDER_INFO
	OutputNSISFolderInfo();
#endif

	Window* window = nullptr;

	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) == 0)
	{
		window = createWindow();

		if (window)
		{
			const bool success = initRenderer(window);
			if (success)
			{
				initIMG();
				initFont();
				initAudio();
				SDL_ShowCursor(false);
			}
			else
				DebugPrint(Error, "Renderer could not be created! SDL Image Error: %s", IMG_GetError());
		}
		else
			DebugPrint(Error, "Window could not be created! SDL Error: %s", SDL_GetError());
	}
	else
		DebugPrint(Error, "SDL could not be initialised! SDL_Error: %s", SDL_GetError());

	return window;
}



// --- Static Functions --- //

void GameSetup::closeSubSystems()
{
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}


void GameSetup::setTutorial(const char* mode)
{
	//const BasicString gameSettingsPath = FileManager::Get()->findFile(FileManager::Configs, "GameSettings");

	//XMLParser parser(gameSettingsPath.c_str());
	//XMLNode tutorialNode = parser.rootChild("Tutorial");
	//tutorialNode.setValue(mode);

	//std::ofstream settingsFile;
	//settingsFile.open(gameSettingsPath.c_str());

	//parser.saveToFile(settingsFile);

	//settingsFile.close();
}



// --- Private Functions --- //

void GameSetup::initIMG()
{
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		DebugPrint(Error, "SDL Image was not initialised! SDL Image Error: %s", IMG_GetError());
	}
}

void GameSetup::initFont()
{
	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		DebugPrint(Error, "SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
	}
}
void GameSetup::initAudio()
{
	// init audio mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		DebugPrint(Error, "SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
	}
}


Window* GameSetup::createWindow()
{
	Window* window = new Window;
	
	const Config* gs = ConfigManager::Get()->GetConfig("GameSettings");

	if(gs->values.GetBool("FitToScreen", false))
	{
		SDL_Rect rect;
		bool success = SDL_GetDisplayUsableBounds(0, &rect);
		if(!success)
			DebugPrint(Error, "%s", SDL_GetError());
		
		const VectorI screenSize(rect.w, rect.h);
		window->init(gs->values.GetString("Title"), screenSize);
	}
	else
	{	
		const int width = gs->values.GetInt("Width");
		const int height = gs->values.GetInt("Height");
		const VectorI screenSize(width, height);
		window->init(gs->values.GetString("Title"), screenSize);
	}

	return window;
}


bool GameSetup::initRenderer(Window* window)
{
	SDL_Renderer* sdlRenderer = window->createRenderer();
	Renderer::Get()->create(sdlRenderer);
	return sdlRenderer != nullptr;
}

