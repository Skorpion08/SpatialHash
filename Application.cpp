#include <cstdio>

#include "Application.h"

Application::Application()
{
}

Application::~Application()
{
}

void Application::Run()
{
	if (!Init())
	{
		printf("Failed to initialize\n");
		return;
	}
	if (!LoadMedia())
	{
		printf("Failed to load media");
	}
	MainLoop();
	Close();
}

bool Application::Init()
{
	// Returns 0 on success
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if (!window.CreateWindow())
	{
		printf("Failed to create window!\n");
		return false;
	}
	
	renderer = SDL_CreateRenderer(window.GetSDL_Window(), -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	return true;
}

bool Application::LoadMedia()
{
	TextureManager::Init(renderer);

	texture1 = TextureManager::Load("resources/smiley.png");
	
	//TextureManager::Load("resources/heart.png");
	//texture2 = TextureManager::Load("resources/circle.png");
	
	//TextureManager::UnLoad("resources/heart.png");
	/*
	std::vector<const char*> missingTextures;
	missingTextures.reserve(textures.size());

	for (auto const& texture : textures)
	{
		//If the SDL_Texture is nullptr add its name to the vector
		if (texture.second == nullptr)
		{
			missingTextures.push_back(texture.first);
		}
	}

	if (missingTextures.empty())
		return true;

	for (const char* name : missingTextures)
	{
		printf("Missing texture's name: %s\n", name);
	}

	return false;
	*/

	return true;
}

void Application::MainLoop()
{
	bool quit = false;
	
	SDL_Event e;
	
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
				break;
			}
		}

		if (quit)
			break;

		//Clear screen
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture1->Get(), nullptr, nullptr);

		//Update screen
		SDL_RenderPresent(renderer);
	}
}

void Application::Close()
{
	TextureManager::UnLoadAll();
	SDL_DestroyRenderer(renderer);
	window.DestroyWindow();

	IMG_Quit();
	SDL_Quit();
}
