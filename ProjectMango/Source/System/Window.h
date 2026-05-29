#pragma once

class Window
{
public:
	Window() : mWindow(nullptr) { };
	~Window();

	void init(const char* title, Vector2D<int> size);

	SDL_Renderer* createRenderer();

	SDL_Window* get() const { return mWindow; }

	VectorF size() const;
	VectorF realSize() const;

	// pretend size, always 1024x1024
	VectorF fakeSize() const { return VectorF(1024, 1024); }

	void setTitle(const char* title);

	VectorF windowToLevel = VectorF::zero();
private:
	SDL_Window* mWindow;
};