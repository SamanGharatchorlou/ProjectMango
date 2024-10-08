#include "pch.h"
#include "Window.h"

void Window::init(const char* title, Vector2D<int> size)
{
	// create window
	mWindow = SDL_CreateWindow(title, 0, 35,
		size.x, size.y, SDL_WINDOW_SHOWN);
}

Window::~Window()
{
	if(mWindow)
		SDL_DestroyWindow(mWindow);
	
	mWindow = nullptr;
}

SDL_Renderer* Window::createRenderer()
{
	 return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED );
}

void Window::setTitle(const char* title)
{
	SDL_SetWindowTitle(mWindow, title);
}

VectorF Window::size() const
{
	return VectorF(1024, 1024);
}

VectorF Window::realSize() const
{
	int width = -1;
	int height = -1;
	SDL_GetWindowSize(mWindow, &width, &height);
	return VectorF((float)width, (float)height);
}
