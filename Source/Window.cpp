#include <cstdio>

#include "Window.h"

bool Window::CreateWindow()
{
	_window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
	if (_window == NULL)
	{
		printf("Could not create an SDL_Window! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		windowSurface = SDL_GetWindowSurface(_window);
	}
	return true;
}

void Window::DestroyWindow()
{
	SDL_DestroyWindow(_window);
}
