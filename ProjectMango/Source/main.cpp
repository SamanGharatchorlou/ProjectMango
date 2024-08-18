#include "pch.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "Game/GameController.h"

//#pragma comment(lib, "SDL2_image")

#include <thread>
#include <iostream>


int main(int argc, char* args[])
{
	GameController GameApp;

	GameApp.run();

	GameApp.free();

	PRINT_MEMORY;

	return 0;
}
