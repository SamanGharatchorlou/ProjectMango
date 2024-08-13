#pragma once

class Window;
struct GameData;

class GameSetup
{
public:
	GameSetup();

	void initFileSystem();
	void initGameData(GameData& game_data);

	void static closeSubSystems();
	void static setTutorial(const char* mode);

private:
	Window* initSDLWindow();
	Window* createWindow();
	bool initRenderer(Window* window);

	void initIMG();
	void initFont();
	void initAudio();
};

