#include <cstdio>

#include "Application.h"
#include "Vector.h"
#include "Shape.h"


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
	textureManager.Init(renderer);

	// Preload all the textures here
	textureManager.Load("resources/smiley.png");
	textureManager.Load("resources/ktot.png");
	textureManager.Load("resources/square.png");
	
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
	/*
	EntityID ent = entityManager.CreateEnitity();
	entityManager.GetRegistry().transforms.emplace(ent, TransformComponent{ 150,150 });
	auto rect = SDL_Rect{ 0,0,100,100 };
	entityManager.GetRegistry().sprites.emplace(ent, SpriteComponent{ nullptr, &rect, TextureManager::Load("resources/square.png") });
	entityManager.GetRegistry().aabbs.emplace(ent, AABB{ &rect });

	for (int i = 0; i < 1000; i++)
	{
		entityManager.CreateEnitity();
	}

	auto rect2 = SDL_Rect{ 0,0,100,100 };
	EntityID ent2 = entityManager.CreateEnitity();
	entityManager.GetRegistry().transforms.emplace(ent2, TransformComponent{ 200,200 });
	entityManager.GetRegistry().sprites.emplace(ent2, SpriteComponent{ nullptr, &rect2, TextureManager::Load("resources/square.png") });
	entityManager.GetRegistry().aabbs.emplace(ent2, AABB{ &rect2 });
	*/

	Shape2D shape;
	shape.center.x = 150;
	shape.center.y = 150;
	shape.vertices.reserve(4);
	shape.vertices.push_back(Point2(100, 100));
	shape.vertices.push_back(Point2(100, 200));
	shape.vertices.push_back(Point2(200, 200));
	shape.vertices.push_back(Point2(200, 100));

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
			/*
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_a)
					entityManager.GetRegistry().transforms[ent].velX = -200;
				else if (e.key.keysym.sym == SDLK_d)
					entityManager.GetRegistry().transforms[ent].velX = 200;
				else if (e.key.keysym.sym == SDLK_w)
					entityManager.GetRegistry().transforms[ent].velY = -200;
				else if (e.key.keysym.sym == SDLK_s)
					entityManager.GetRegistry().transforms[ent].velY = 200;
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
			*/
			if (e.type == SDL_KEYDOWN)
			{
				if(e.key.keysym.sym == SDLK_RIGHT)
					shape.ApplyRotation(0.1);
				else if (e.key.keysym.sym == SDLK_LEFT)
					shape.ApplyRotation(-0.1);

				/*
				for (auto& vert : shape.vertices)
				{
					printf("x: %f y: %f\n", vert.x, vert.y);
				}
				printf("\n");
				*/
			}
		}

		if (quit)
			break;
		Uint64 start = SDL_GetPerformanceCounter();

		//Clear screen
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		
		currentUpdate = SDL_GetTicks();

		deltaTime = (currentUpdate - lastUpdate) * 0.001f;
		//printf("deltatime %f\n", deltaTime);
		transformSystem.Update(deltaTime);
		collisionSystem.Update();
		//collisionSystem.SolveCollisions();
		spritesSystem.Update();
		spritesSystem.Render();
		
		//shape.ApplyRotation(1*deltaTime);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		for (int i = 0; i < shape.vertices.size(); i++)
		{
			if (i+1 == shape.vertices.size())
			{
				SDL_RenderDrawLine(renderer, shape.vertices[i].x, shape.vertices[i].y, shape.vertices[0].x, shape.vertices[0].y);
				break;
			}
			SDL_RenderDrawLine(renderer, shape.vertices[i].x, shape.vertices[i].y, shape.vertices[i+1].x, shape.vertices[i + 1].y);
		}
		

		lastUpdate = currentUpdate;
		//printf("velocity Y %f\n", entityManager::GetRegistry().transforms[smiley].velY);
		//Update screen
		SDL_RenderPresent(renderer);

		Uint64 end = SDL_GetPerformanceCounter();
		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
		//printf("FPS: %f\n", 1.0f / elapsed);
		
	}
}

void Application::Close()
{
	textureManager.UnLoadAll();
	SDL_DestroyRenderer(renderer);
	window.DestroyWindow();

	IMG_Quit();
	SDL_Quit();
}
