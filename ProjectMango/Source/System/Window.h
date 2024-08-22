#pragma once

class Window
{
public:
	Window() : mWindow(nullptr) { };
	~Window();

	void init(const char* title, Vector2D<int> size);

	SDL_Renderer* createRenderer();

	SDL_Window* get() const { return mWindow; }

	// pretend size, always 1024x1024
	VectorF size() const;

	void setTitle(const char* title);


private:
	SDL_Window* mWindow;
};