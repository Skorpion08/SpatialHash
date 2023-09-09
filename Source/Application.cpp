#include <cstdio>
#include <chrono>
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
		throw std::runtime_error("Failed to initalize");
	}
	LoadMedia();
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
	textureManager.Init(renderer);

	// Preload all the textures here
	textureManager.Load("resources/smiley.png");
	textureManager.Load("resources/ktot.png");
	textureManager.Load("resources/square.png");
	textureManager.Load("resources/sad block.png");
	textureManager.Load("resources/swastyka.png");
	textureManager.Load("resources/fence.png");

	return true;
}

void Application::MainLoop()
{
	bool quit = false;
	
	SDL_Event e;

	float v = 1000.0f;

	float ad = 1000.0f;

	float b = 3;

	float d = 3;

	bool keyPressed = false;


	EntityID fence = entityManager.CreateEnitity();
	entityManager.AddTransform(fence);
	SDL_Rect rectum = { 0,0, 4176 * 2, 960 * 2 };
	entityManager.AddSprite(fence, nullptr, &rectum, textureManager.Load("resources/fence.png"));


	EntityID square2 = entityManager.CreateEnitity();
	entityManager.AddTransform(square2, { 600,600 });
	entityManager.AddKinematics(square2);
	SDL_Rect dst2(100, 100, 100, 100);
	entityManager.AddSprite(square2, nullptr, &dst2, textureManager.Load("resources/square.png"));
	OBB obb2(100, 100);
	
	entityManager.AddRigidbody(square2, 60, true, 490);
	entityManager.AddCollision(square2, &obb2);

	EntityID square3 = entityManager.CreateEnitity();
	entityManager.AddTransform(square3, { 400,600 });
	entityManager.AddKinematics(square3);
	SDL_Rect dst3(100, 100, 100, 100);
	entityManager.AddSprite(square3, nullptr, &dst3, textureManager.Load("resources/square.png"));
	AABB aabb1(100, 100);

	entityManager.AddRigidbody(square3, 50, true, 490);

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
	EntityID rectangle = entityManager.CreateEnitity(Static);
	entityManager.AddTransform(rectangle, { 400,700 });
	entityManager.AddKinematics(rectangle);
	SDL_Rect dstrect(0, 0, 100000, 100);
	entityManager.AddSprite(rectangle, nullptr, &dstrect, textureManager.Load("resources/square.png"));
	AABB aabb2(100000, 100);

	entityManager.AddRigidbody(rectangle, 0, false);
	entityManager.AddCollision(rectangle, &aabb2);

	/*
	EntityID rectangle2 = entityManager.CreateEnitity(Dynamic);
	entityManager.AddTransform(rectangle2, { 400, 0});
	entityManager.AddKinematics(rectangle2, {0,100});
	SDL_Rect dstrect2(0, 0, 10000, 100);
	entityManager.AddSprite(rectangle2, nullptr, &dstrect2, textureManager.Load("resources/square.png"));
	AABB aabb3(100000, 100);

	entityManager.AddRigidbody(rectangle2, 1e6, false);
	entityManager.AddCollision(rectangle2, &aabb3);
	*/
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

	EntityID border = entityManager.CreateEnitity(Static);
	entityManager.AddTransform(border, { 50000, 700 });
	SDL_Rect bordeah = { 0,0,100,10000 };
	//entityManager.AddSprite(border, nullptr, &bordeah, textureManager.Load("resources/square.png"));
	entityManager.Add<Sprite>(border, nullptr, &bordeah, textureManager.Load("resources/square.png"));
	AABB borderaa(100, 10000);
	entityManager.AddCollision(border, &borderaa);
	entityManager.AddRigidbody(border, 0, false);


	Archetype arch;
	arch.table.push_back(Column(typeid(Transform), sizeof(Transform)));
	arch.table.push_back(Column(typeid(Sprite), sizeof(Sprite)));
	arch.table.push_back(Column(typeid(Kinematics), sizeof(Kinematics)));

	arch.table[0].Insert<Transform>(Transform());
	arch.table[0].Insert<Transform>(Transform());
	arch.table[0].Insert<Transform>(Transform());
	arch.table[0].Insert<Transform>(Transform());
	//arch.table[0].Insert<Transform>(Transform());

	arch.table[1].Insert<Sprite>(Sprite());
	arch.table[1].Insert<Sprite>(Sprite());
	arch.table[1].Insert<Sprite>(Sprite());

	arch.table[2].Insert<Kinematics>(Kinematics());
	arch.table[2].Insert<Kinematics>(Kinematics());

	arch.table[2].Get<Kinematics>(0)->vel.x = 10;
	printf("\t\t\n %d", sizeof(arch.table[0].elements[0]) * arch.table[0].elements.size() + sizeof(arch.table[1].elements[0]) * arch.table[1].elements.size() + sizeof(arch.table[2].elements[0]) * arch.table[2].elements.size());

	/*
	std::vector<SDL_Rect> s;
	std::vector<AABB> c;
	
	for (int i = 0; i < 50; ++i)
	{
		SDL_Rect sqe = { 0,0,100,100 };
		s.push_back(sqe);
		AABB av(100, 100);
		c.push_back(av);
	}
	for (int i = 0; i < 50; ++i)
	{
		EntityID ent = entityManager.CreateEnitity();
		entityManager.AddTransform(ent, { (float)10*i, 0 });
		entityManager.AddKinematics(ent);
		entityManager.AddSprite(ent, nullptr, &s[i], textureManager.Load("resources/square.png"));
		entityManager.AddCollision(ent, &c[i]);
		entityManager.AddRigidbody(ent, 100, true, 100, 0.4);
		printf("%f\n", (i * 100) / 1e6);

	}
	*/
	// Camera code
	Vector2 camera;

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
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_LEFT)
				{
					entityManager.GetRegistry().kinematics[square2].acc.x += -v;
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					entityManager.GetRegistry().kinematics[square2].acc.x += v;
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					entityManager.GetRegistry().kinematics[square2].acc.y += -v;
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					entityManager.GetRegistry().kinematics[square2].acc.y += v;
					keyPressed = true;
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
				if (e.key.keysym.sym == SDLK_LEFT)
				{
					entityManager.GetRegistry().kinematics[square2].acc.x -= -v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					entityManager.GetRegistry().kinematics[square2].acc.x -= v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					entityManager.GetRegistry().kinematics[square2].acc.y -= -v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					entityManager.GetRegistry().kinematics[square2].acc.y -= v;
					keyPressed = false;
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
		deltaTime = (currentUpdate - lastUpdate) * 0.001;
		//deltaTime *= 4;
		//if (!keyPressed)
		//{
		//	entityManager.GetRegistry().kinematics[square2].vel.x /= 1 + d * deltaTime;
		//	entityManager.GetRegistry().kinematics[square2].vel.y /= 1 + d * deltaTime;
		//}
		//printf("deltatime %f\n", deltaTime);
		transformSystem.Update(deltaTime);
		collisionSystem.Update();
		collisionSystem.SolveCollisions();

		// Camera updating
		camera.x = entityManager.GetRegistry().transforms[square2].pos.x - window.w / 2;
		camera.y = entityManager.GetRegistry().transforms[square2].pos.y - window.h / 2;

		//entityManager.GetRegistry().transforms[rectangle].pos = { 400, 700 };
		//entityManager.GetRegistry().kinematics[rectangle].vel = { 0,0 };
		//printf("vel: %f\n", entityManager.GetRegistry().kinematics[square2].vel.x);
		//printf("camera: %f %f\n", camera.x, camera.y);
		//printf("acc %f %f\n", entityManager.GetRegistry().kinematics[square2].acc.x, entityManager.GetRegistry().kinematics[square2].acc.y);
		spritesSystem.Update({ static_cast<float>(camera.x), static_cast<float>(camera.y) });
		spritesSystem.Render();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		/*
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
		*/
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

		//printf("fps: %f\n", 1/deltaTime);
		//printf("deltatime: %f\n", deltaTime);

		/* frame cap
		if (1000 / 100 > deltaTime)
		{
			SDL_Delay(1000 / 100 - deltaTime);
		}
		*/
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
