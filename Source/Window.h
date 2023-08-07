#pragma once
#include <SDL.h>
#include <SDL_image.h>

class Window
{
public:
	bool CreateWindow();

	void DestroyWindow();

	SDL_Window* GetSDL_Window() { return _window; }

	SDL_Surface* windowSurface;

	int w, h;
private:
	SDL_Window* _window;
};