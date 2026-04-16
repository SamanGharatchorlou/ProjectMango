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
	BasicString path;
	FileManager::Get()->FindFile(FileManager::Configs, "GameSettings", path);
	ASSERT(path.c_str() != nullptr, "No game settings");
	cm->Parse("GameSettings");

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

				//SDL_CaptureMouse(SDL_TRUE);
				SDL_ShowCursor(SDL_DISABLE);
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

	if(gs->data.GetBool("fit_to_screen", false))
	{
		SDL_Rect rect;
		bool success = SDL_GetDisplayUsableBounds(0, &rect) == 0;
		if(!success)
			DebugPrint(Error, "%s", SDL_GetError());
		
		VectorI screen_size(rect.w, rect.h);
		// account for the top bar
		screen_size.y -= 32;

		VectorF default_ratio(-1.0f, -1.0f);
		VectorF screen_ratio = gs->data.GetVector("screen_ratio", default_ratio);
		if (screen_ratio != default_ratio)
		{
			// set y relative to x
			if (screen_size.x <= screen_size.y)
			{
				screen_size.y = (screen_size.x / screen_ratio.x) * screen_ratio.y;
			}
			// set x relative to u
			else
			{
				screen_size.x = (screen_size.y / screen_ratio.y) * screen_ratio.x;
			}
		}

		window->init(gs->data.GetString("Title"), screen_size);
	}
	else
	{	
		const int width = gs->data.GetInt("size_x");
		const int height = gs->data.GetInt("size_y");
		const VectorI screen_size(width, height);
		window->init(gs->data.GetString("title"), screen_size);
	}

	return window;
}


bool GameSetup::initRenderer(Window* window)
{
	SDL_Renderer* sdlRenderer = window->createRenderer();
	Renderer::Get()->create(sdlRenderer);
	return sdlRenderer != nullptr;
}

