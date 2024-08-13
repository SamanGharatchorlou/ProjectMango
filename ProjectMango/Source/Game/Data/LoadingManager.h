#pragma once

#include "LoadingBar.h"
#include "Graphics/Renderable.h"

class UIManager;
class Audio;

// Implementing this means the loading time will probably increase by around 10%-15%
// This was from very quick testing, may vary.
class LoadingManager : public Renderable
{
public:
	LoadingManager();
	~LoadingManager();

	static LoadingManager* Get();

	void init();
	void exit();

	void successfullyLoaded(const BasicString& filePath);
	float loadedPercentage();

	void earlyExit();
	bool shouldEarlyExit() const { return mEarlyExit; }

	void setLoadingAssets(bool isLoading) { mLoadingAssets = isLoading; }
	bool isLoadingAssets() const { return mLoadingAssets; }

	void update();
	void render();

private:

	uintmax_t mTotalFileSizes;
	uintmax_t mLoadedFileSizes;

	//UIManager* mUI;

	bool mEarlyExit;
	bool mLoadingAssets;
};