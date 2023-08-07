#include <cstdio>

#include "Application.h"
#include "Vector.h"
#include "Shape.h"


Application::Application()
{
	window.w = 1000;
	window.h = 1000;
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
	textureManager.Load("resources/sad block.png");
	textureManager.Load("resources/swastyka.png");

	return true;
}

void Application::MainLoop()
{
	bool quit = false;
	
	SDL_Event e;

	EntityID triangle = entityManager.CreateEnitity();
	entityManager.AddTransform(triangle, {100,100});
	entityManager.AddKinematics(triangle);
	SDL_Rect dst1(50, 50, 100, 100);
	//entityManager.AddSprite(triangle, nullptr, &dst1, textureManager.Load("resources/triangle.png"));
	Polygon obb1;
	obb1.center = { 100,100 };
	obb1.vertices.reserve(3);
	obb1.sides.resize(3);
	obb1.vertices.emplace_back(100, 50);
	obb1.vertices.emplace_back(50, 150);
	obb1.vertices.emplace_back(150, 150);

	//entityManager.AddRigidbody(triangle, 10, false);
	entityManager.AddCollision(triangle, &obb1);

	float v = 500.0f;

	float ad = 500.0f;

	float b = 3;

	EntityID square2 = entityManager.CreateEnitity();
	entityManager.AddTransform(square2, { 200,100 });
	entityManager.AddKinematics(square2);
	SDL_Rect dst2(100, 100, 100, 100);
	entityManager.AddSprite(square2, nullptr, &dst2, textureManager.Load("resources/square.png"));
	entityManager.AddRigidbody(square2, 10000, false);
	OBB obb2(100, 100);
	
	//entityManager.AddRigidbody(square2, 10, false);
	entityManager.AddCollision(square2, &obb2);

	EntityID square3 = entityManager.CreateEnitity();
	entityManager.AddTransform(square3, { 400,400 });
	entityManager.AddKinematics(square3);
	SDL_Rect dst3(100, 100, 100, 100);
	entityManager.AddSprite(square3, nullptr, &dst3, textureManager.Load("resources/square.png"));
	AABB aabb1(100, 100);

	entityManager.AddRigidbody(square3, 12, true, 50);

	entityManager.AddCollision(square3, &aabb1);

	/*
	EntityID swastyka = entityManager.CreateEnitity();
	entityManager.AddTransform(swastyka);
	entityManager.AddKinematics(swastyka, { 0,0 }, { 0,0 }, 0, 1);
	SDL_Rect dsts(0, 0, 400, 400);
	entityManager.AddSprite(swastyka, nullptr, &dsts, textureManager.Load("resources/swastyka.png"));

	OBB swastik(400, 400);
	entityManager.AddCollision(swastyka, &swastik);
	*/
	EntityID rectangle = entityManager.CreateEnitity();
	entityManager.AddTransform(rectangle, { 400,700 });
	entityManager.AddKinematics(rectangle);
	SDL_Rect dstrect(0, 0, 400, 100);
	entityManager.AddSprite(rectangle, nullptr, &dstrect, textureManager.Load("resources/square.png"));
	AABB aabb2(400, 100);

	entityManager.AddRigidbody(rectangle, 9000, false);
	entityManager.AddCollision(rectangle, &aabb2);
	/*
	EntityID square4 = entityManager.CreateEnitity();
	entityManager.AddTransform(square4, { 400,400 });
	SDL_Rect dst4(100, 100, 100, 100);
	entityManager.AddSprite(square4, nullptr, &dst4, textureManager.Load("resources/triangle.png"));
	AABB aabb2(100, 100, { 400,400 });

	entityManager.GetRegistry().collisionComponents[square4].collider = &aabb2;

	EntityID square5 = entityManager.CreateEnitity();
	entityManager.AddTransform(square5, { 400,400 });
	SDL_Rect dst5(100, 100, 100, 100);
	entityManager.AddSprite(square5, nullptr, &dst5, textureManager.Load("resources/triangle.png"));
	AABB aabb3(100, 100, { 400,400 });

	entityManager.GetRegistry().collisionComponents[square5].collider = &aabb3;
	*/

	// Camera code
	Vector2 camera;

	Uint32 lastUpdate = SDL_GetTicks();
	Uint32 currentUpdate = 0;
	float highest = std::numeric_limits<float>::max();
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
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_a)
				{
					entityManager.GetRegistry().kinematics[triangle].vel.x += -v;
				}
				else if (e.key.keysym.sym == SDLK_d)
				{
					entityManager.GetRegistry().kinematics[triangle].vel.x += v;
				}
				else if (e.key.keysym.sym == SDLK_w)
				{
					entityManager.GetRegistry().kinematics[triangle].vel.y += -v;
				}
				else if (e.key.keysym.sym == SDLK_s)
				{
					entityManager.GetRegistry().kinematics[triangle].vel.y += v;
				}
				else if (e.key.keysym.sym == SDLK_q)
				{
					entityManager.GetRegistry().kinematics[triangle].angularVel = -b;
				}
				else if (e.key.keysym.sym == SDLK_e)
				{
					entityManager.GetRegistry().kinematics[triangle].angularVel = b;
				}
				else if (e.key.keysym.sym == SDLK_LEFT)
				{
					entityManager.GetRegistry().kinematics[square2].vel.x += -ad;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					entityManager.GetRegistry().kinematics[square2].vel.x += ad;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					entityManager.GetRegistry().kinematics[square2].vel.y += -ad;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					entityManager.GetRegistry().kinematics[square2].vel.y += ad;
				}
				else if (e.key.keysym.sym == SDLK_o)
				{
					entityManager.GetRegistry().kinematics[square2].angularVel = -b;
				}
				else if (e.key.keysym.sym == SDLK_p)
				{
					entityManager.GetRegistry().kinematics[square2].angularVel = b;
				}
			}
			else if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_a)
				{
					entityManager.GetRegistry().kinematics[triangle].vel.x -= -v;
				}
				else if (e.key.keysym.sym == SDLK_d)
				{
					entityManager.GetRegistry().kinematics[triangle].vel.x -= v;
				}
				else if (e.key.keysym.sym == SDLK_w)
				{
					entityManager.GetRegistry().kinematics[triangle].vel.y -= -v;
				}
				else if (e.key.keysym.sym == SDLK_s)
				{
					entityManager.GetRegistry().kinematics[triangle].vel.y -= v;
				}
				else if (e.key.keysym.sym == SDLK_q)
				{
					entityManager.GetRegistry().kinematics[triangle].angularVel = 0;
				}
				else if (e.key.keysym.sym == SDLK_e)
				{
					entityManager.GetRegistry().kinematics[triangle].angularVel = 0;
				}
				else if (e.key.keysym.sym == SDLK_LEFT)
				{
					entityManager.GetRegistry().kinematics[square2].vel.x -= -ad;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					entityManager.GetRegistry().kinematics[square2].vel.x -= ad;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					entityManager.GetRegistry().kinematics[square2].vel.y -= -ad;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					entityManager.GetRegistry().kinematics[square2].vel.y -= ad;
				}
				else if (e.key.keysym.sym == SDLK_o)
				{
					entityManager.GetRegistry().kinematics[square2].angularVel = 0;
				}
				else if (e.key.keysym.sym == SDLK_p)
				{
					entityManager.GetRegistry().kinematics[square2].angularVel = 0;
				}
			}
			
		}

		if (quit)
			break;

		//Clear screen
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		
		currentUpdate = SDL_GetTicks();
		// in seconds
		deltaTime = (currentUpdate - lastUpdate) * 0.001f;
		//printf("deltatime %f\n", deltaTime);
		transformSystem.Update(deltaTime);
		collisionSystem.Update();
		collisionSystem.SolveCollisions();

		// Camera updating
		camera.x = entityManager.GetRegistry().transforms[square2].pos.x - window.w / 2;
		camera.y = entityManager.GetRegistry().transforms[square2].pos.y - window.h / 2;

		entityManager.GetRegistry().transforms[rectangle].pos = { 400, 700 };
		entityManager.GetRegistry().kinematics[rectangle].vel = { 0,0 };
		//printf("pos: %f %f\n", entityManager.GetRegistry().kinematics[rectangle].vel.x, entityManager.GetRegistry().kinematics[rectangle].vel.y);
		//printf("camera: %f %f\n", camera.x, camera.y);
		//printf("acc %f\n", entityManager.GetRegistry().kinematics[square3].acc.y);
		spritesSystem.Update({ static_cast<float>(camera.x), static_cast<float>(camera.y) });
		spritesSystem.Render();

		float height = entityManager.GetRegistry().transforms[square3].pos.y;
		if (height < highest)
		{
			highest = height;
			printf("highest: %f\n", highest);
		}
		//printf("h: %f\n", height);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		
		for (int i = 0; i < obb1.vertices.size(); i++)
		{
			auto& shape = obb1;
			if (i + 1 == shape.vertices.size())
			{
				SDL_RenderDrawLine(renderer, shape.vertices[i].x - camera.x, shape.vertices[i].y - camera.y, shape.vertices[0].x - camera.x, shape.vertices[0].y - camera.y);
				break;
			}
			SDL_RenderDrawLine(renderer, shape.vertices[i].x - camera.x, shape.vertices[i].y - camera.y, shape.vertices[i + 1].x - camera.x, shape.vertices[i + 1].y - camera.y);
		}
		/*
		for (int i = 0; i < obb2.vertices.size(); i++)
		{
			auto& shape = obb2;
			if (i + 1 == shape.vertices.size())
			{
				SDL_RenderDrawLine(renderer, shape.vertices[i].x - camera.x, shape.vertices[i].y - camera.y, shape.vertices[0].x - camera.x, shape.vertices[0].y - camera.y);
				break;
			}
			SDL_RenderDrawLine(renderer, shape.vertices[i].x - camera.x, shape.vertices[i].y - camera.y, shape.vertices[i + 1].x - camera.x, shape.vertices[i + 1].y - camera.y);
		}
		*/
		/*
		for (int i = 0; i < obb2.vertices.size(); i++)
		{
			if (i + 1 == obb2.vertices.size())
			{
				SDL_RenderDrawLine(renderer, obb2.vertices[i].x, obb2.vertices[i].y, obb2.vertices[0].x, obb2.vertices[0].y);
				break;
			}
			SDL_RenderDrawLine(renderer, obb2.vertices[i].x, obb2.vertices[i].y, obb2.vertices[i + 1].x, obb2.vertices[i + 1].y);
		}
		*/
		lastUpdate = currentUpdate;
		//Update screen
		SDL_RenderPresent(renderer);
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
