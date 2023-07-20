#include <cstdio>

#include "Application.h"
#include "Vector.h"


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
	
	// Added vsync because ima to lazy to add frame cap or deltatime
	renderer = SDL_CreateRenderer(window.GetSDL_Window(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

	// Preload all the textures here
	TextureManager::Load("resources/smiley.png");
	TextureManager::Load("resources/ktot.png");
	TextureManager::Load("resources/square.png");
	
	//manager.CreateEnitity();
	

	return true;
}

void Application::MainLoop()
{
	bool quit = false;
	
	SDL_Event e;

	// ECS TEST
	/*
	EntityID smiley = entityManager::CreateEnitity();
	TransformComponent tc{ 0,300,20,-280,1, 0};
	entityManager::GetRegistry().transforms.insert({ smiley, tc });
	SDL_Rect dst{ 0,0, 400,400 };
	SpriteComponent sc{ nullptr, &dst, TextureManager::Load("resources/ktot.png")};
	entityManager::GetRegistry().sprites.insert({ smiley, sc});

	Vector2 vector(10, 20);
	Vector2 normal = vector.GetNormal();
	printf("vector: %f\n", Vector::DotProduct(vector, normal));
	*/
	EntityID ent = entityManager.CreateEnitity();
	entityManager.GetRegistry().transforms.emplace(ent, TransformComponent{ 150,150 });
	auto rect = SDL_Rect{ 0,0,100,100 };
	entityManager.GetRegistry().sprites.emplace(ent, SpriteComponent{ nullptr, &rect, TextureManager::Load("resources/square.png") });
	entityManager.GetRegistry().aabbs.emplace(ent, AABB{ &rect });

	auto rect2 = SDL_Rect{ 0,0,100,100 };
	EntityID ent2 = entityManager.CreateEnitity();
	entityManager.GetRegistry().transforms.emplace(ent2, TransformComponent{ 200,200 });
	entityManager.GetRegistry().sprites.emplace(ent2, SpriteComponent{ nullptr, &rect2, TextureManager::Load("resources/square.png") });
	entityManager.GetRegistry().aabbs.emplace(ent2, AABB{ &rect2 });

	Uint32 lastUpdate = SDL_GetTicks();
	Uint32 currentUpdate = 0;

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
			/*
			if (e.type == SDL_KEYDOWN)
			{
				entityManager::GetRegistry().transforms[smiley].velY += -80;
				entityManager::GetRegistry().transforms[smiley].accY += -10;
				printf("HEY");
				if (e.key.keysym.sym == SDLK_r)
				{
					entityManager::GetRegistry().transforms[smiley].posX = 0;
					entityManager::GetRegistry().transforms[smiley].posY = 300;
					entityManager::GetRegistry().transforms[smiley].velX = 20;
					entityManager::GetRegistry().transforms[smiley].velY = -280;
					entityManager::GetRegistry().transforms[smiley].accX = 1;
					entityManager::GetRegistry().transforms[smiley].accY = 0;
				}
			}
			*/
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_a)
					entityManager.GetRegistry().transforms[ent].velX = -100;
				else if (e.key.keysym.sym == SDLK_d)
					entityManager.GetRegistry().transforms[ent].velX = 100;
				else if (e.key.keysym.sym == SDLK_w)
					entityManager.GetRegistry().transforms[ent].velY = -100;
				else if (e.key.keysym.sym == SDLK_s)
					entityManager.GetRegistry().transforms[ent].velY = 100;
			}
			else if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_a)
					entityManager.GetRegistry().transforms[ent].velX = 0;
				else if (e.key.keysym.sym == SDLK_d)
					entityManager.GetRegistry().transforms[ent].velX = 0;
				else if (e.key.keysym.sym == SDLK_w)
					entityManager.GetRegistry().transforms[ent].velY = 0;
				else if (e.key.keysym.sym == SDLK_s)
					entityManager.GetRegistry().transforms[ent].velY = 0;
			}
		}

		if (quit)
			break;
		//Uint64 start = SDL_GetPerformanceCounter();

		//Clear screen
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		
		currentUpdate = SDL_GetTicks();

		deltaTime = (currentUpdate - lastUpdate) * 0.001f;
		//printf("deltatime %f\n", deltaTime);
		transformSystem.Update(deltaTime);
		collisionSystem.Update();
		collisionSystem.SolveCollisions();
		spritesSystem.Update();
		spritesSystem.Render();

		lastUpdate = currentUpdate;
		//printf("velocity Y %f\n", entityManager::GetRegistry().transforms[smiley].velY);
		//Update screen
		SDL_RenderPresent(renderer);

		//Uint64 end = SDL_GetPerformanceCounter();
		//float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
		//printf("FPS: %f\n", 1.0f / elapsed);
		
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
