#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <vector>

#include "Window.h"
#include "TextureManager.h"

class Application
{
public:
	Application();
	virtual ~Application();

	void Run();
	
	bool Init();

	bool LoadMedia();

	void MainLoop();

	void Close();

private:
	Window window;

	SDL_Renderer* renderer;
	Texture* texture1;
	Texture* texture2;
};