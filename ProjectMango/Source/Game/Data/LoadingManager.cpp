#include "pch.h"
#include "LoadingManager.h"

#include "Graphics/Renderer.h"
#include "UI/UIManager.h"
#include "UI/Screens/LoadingScreen.h"
#include "UI/Elements/UISlider.h"
#include "UI/Elements/UITextBox.h"

static LoadingManager* s_loader = nullptr;

LoadingManager::LoadingManager() : mTotalFileSizes(0), mLoadedFileSizes(0), mEarlyExit(false), mLoadingAssets(false) { s_loader = this; }
LoadingManager::~LoadingManager() { s_loader = nullptr; }
LoadingManager* LoadingManager::Get() { return s_loader; }

void LoadingManager::init()
{
	// count up all the files to load
	std::vector<FileManager::Folder> folders;
	folders.push_back(FileManager::Images);
	folders.push_back(FileManager::Audio);
	for (FileManager::Folder folder : folders)
	{
		std::vector<BasicString> filePaths = FileManager::Get()->allFilesInFolder(folder);

		for (int i = 0; i < filePaths.size(); i++)
		{
			fs::path fileSystemPath(filePaths[i].c_str());
			if (fs::exists(fileSystemPath))
			{
				mTotalFileSizes += fs::file_size(fileSystemPath);
				continue;
			}

			DebugPrint(Warning, "The file at path '%s' does not exist. Cannot add to 'to be loaded'", filePaths[i].c_str());
		}
	}
}

void LoadingManager::exit()
{
	mTotalFileSizes = 0;
	mLoadedFileSizes = 0;

	mEarlyExit = false;
	mLoadingAssets = false;
}


void LoadingManager::update()
{
	Screen* screen = GameData::Get().uiManager->getActiveScreen();
	if (UISlider* loadingBar = screen->getSlider("LoadingSlider"))
	{
		float value = loadedPercentage();
		loadingBar->setSliderValue(loadedPercentage());
	}
}


void LoadingManager::render()
{
	//Renderer* renderer = Renderer::Get();
	//renderer->lock();

	//// Clear screen
	//SDL_SetRenderDrawColor(renderer->sdlRenderer(), 0, 0, 0, 255);
	//SDL_RenderClear(renderer->sdlRenderer());

	//GameData::Get().uiManager->getActiveScreen()->render();

	//// Update window surface
	//SDL_RenderPresent(renderer->sdlRenderer());
	//renderer->unlock();
}


void LoadingManager::earlyExit() 
{ 
	mEarlyExit = true;

	Screen* screen = GameData::Get().uiManager->getActiveScreen();

	UITextBox* text = screen->findTextBox("LoadingText");
	text->setText("Exiting Game...");
	text->align();
}


void LoadingManager::successfullyLoaded(const BasicString& filePath)
{
	fs::path fileSystemPath(filePath.c_str());
	if (fs::exists(fileSystemPath))
	{
		if (fs::is_directory(fileSystemPath))
		{
			std::vector<BasicString> files = FileManager::Get()->allFilesInFolder(fileSystemPath);

			for(const BasicString& file : files)
				mLoadedFileSizes += fs::file_size(fs::path(file.c_str()));
		}
		else
		{
			mLoadedFileSizes += fs::file_size(fileSystemPath);
		}
	}
	else
	{
		DebugPrint(Warning, "The file at path '%s' does not exist. Cannot add to 'already loaded'", filePath.c_str());
	}
}

float LoadingManager::loadedPercentage()
{
	return (float)mLoadedFileSizes / (float)mTotalFileSizes;
}
