#pragma once
#include <mutex>

class Renderer
{
public:
	static Renderer* Get();

	// Only used while loading for thread safety
	void lock();
	void unlock();

	void free() { SDL_DestroyRenderer(mRenderer); }
	void create(SDL_Renderer* renderer) { mRenderer = renderer; }

	SDL_Renderer* sdlRenderer() const { return mRenderer; }

	void setScale(float scale);

private:
	Renderer();
	~Renderer() { }

	SDL_Renderer* mRenderer;
	SDL_SpinLock mLoadingLock;

	std::mutex mMutex;
};