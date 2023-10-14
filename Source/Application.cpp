#include <cstdio>
#include <chrono>
#include <random>

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

	float v = 1000;

	float ad = 1000.0f;

	float b = 3;

	float d = 3;

	bool keyPressed = false;

	int frame = 0;

	struct A {
		int a = 0;
		int b = 0;
		int c = 0;
	};
	struct B {
		//B(int _x) : x(_x) { std::cout << "created\n"; }
		int x;
		//~B() { std::cout << "destroyed\n"; }
	};
	struct C { int x; };
	struct D { int x; };
	struct E { int x; };
	struct F { int x, y, z; }; struct G { int x, y, z, w; }; struct H { int x, y; };

	std::random_device device;
	std::mt19937 mt(device());
	std::uniform_int_distribution<int> dist(-100, 100);

	EntityID ceiling = ECS::NewEnitity();
	AddData(ceiling, Transform, Vector2(0, 0));
	AddData(ceiling, Kinematics);
	AddData(ceiling, Sprite, 1000, 100, nullptr, textureManager.Load("resources/square.png"));
	AddData(ceiling, Collision, new OBB(1000, 100));
	AddData(ceiling, Rigidbody, 10000000);

	EntityID floor = ECS::NewEnitity();
	AddData(floor, Transform, Vector2(0, 5000));
	AddData(floor, Kinematics);
	AddData(floor, Sprite, 1000, 100, nullptr, textureManager.Load("resources/square.png"));
	AddData(floor, Collision, new OBB(1000, 100));
	AddData(floor, Rigidbody, 10000000);

	EntityID lwall = ECS::NewEnitity();
	AddData(lwall, Transform, Vector2(-100, 0));
	AddData(lwall, Kinematics);
	AddData(lwall, Sprite, 100, 10000, nullptr, textureManager.Load("resources/square.png"));
	AddData(lwall, Collision, new OBB(100, 10000));
	AddData(lwall, Rigidbody, 10000000);

	EntityID rwall = ECS::NewEnitity();
	AddData(rwall, Transform, Vector2(100, 0));
	AddData(rwall, Kinematics);
	AddData(rwall, Sprite, 100, 10000, nullptr, textureManager.Load("resources/square.png"));
	AddData(rwall, Collision, new OBB(100, 10000));
	AddData(rwall, Rigidbody, 10000000);

	EntityID e1 = ECS::NewEnitity();
	//EntityID e2 = ECS::NewEnitity();

	AddData(e1, Transform);
	AddData(e1, Kinematics);
	AddData(e1, Sprite, 100, 100, nullptr, textureManager.Load("resources/square.png"));
	AddData(e1, Collision, new OBB(100, 100));
	AddData(e1, Rigidbody, 10, 1, 0.4, 0.3, 0.1);

	EntityID e0 = ECS::NewEnitity();
	AddData(e0, Transform, Vector2(50, 50));
	AddData(e0, Kinematics, Vector2(), Vector2(0,1000));
	AddData(e0, Sprite, 100, 100, nullptr, textureManager.Load("resources/square.png"));
	AddData(e0, Collision, new OBB(100, 100));
	AddData(e0, Rigidbody, 15, 1, 0.3, 0.1, 0.1);
#if 0
	for (int i = 0; i < 1000000; ++i)
	{
		EntityID e = ecs.CreateEnitity();
		ecs.Add<A>(e, dist(mt), dist(mt), dist(mt));
		ecs.Add<B>(e, dist(mt));
		ecs.Add<C>(e, dist(mt));
		ecs.Add<D>(e, dist(mt));
		ecs.Add<E>(e, dist(mt));
		ecs.Add<F>(e, dist(mt), dist(mt), dist(mt));
		ecs.Add<G>(e, dist(mt), dist(mt), dist(mt));
		ecs.Add<H>(e, dist(mt), dist(mt));
	}
	auto start = std::chrono::high_resolution_clock::now();
	std::vector<Archetype*> query = ecs.Query<A, B, C, D, F>();
	auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double, std::milli> duration = end - start; std::cout << "Querying took: " << duration.count() << " ms\n";
	start = std::chrono::high_resolution_clock::now();
	int sum = 0;
	componentIndex;
	for(int j = 0; j < query.size(); ++j)
	{
		A* aa = query[j]->columns[query[j]->type.FindIndexFor(ecs.GetID<A>())].Get<A>(0);
		B* bb = query[j]->columns[query[j]->type.FindIndexFor(ecs.GetID<B>())].Get<B>(0);
		for (int i = 0; i < query[j]->entityCount; ++i)
		{
			aa[i].a += 3;
			aa[i].b += 10;
			aa[i].c += 12;
			bb[i].x += 1;
			sum += aa[i].a + aa[i].b + aa[i].c + bb[i].x*2;

			//sum += aa[i].b;
			//sum += aa[i].c;
			//sum += bb[i].x;
			// 
			//std::cout << query[j]->id_table[i] << '\n';
			//std::cout << "\tA"<< '\n';
			//std::cout << "\t\ta: " << aa[i].a << '\n';
			//std::cout << "\t\tb: " << aa[i].b << '\n';
			//std::cout << "\t\tc: " << aa[i].c << '\n';
			//std::cout << "\tB" << '\n';
			//std::cout << "\t\tx: " << bb[i].x << '\n';
		}
	}
	end = std::chrono::high_resolution_clock::now(); duration = end - start; std::cout << "Processing took: " << duration.count() << " ms\n";
	std::cout << sum;
#endif

	// Camera code
	Vector2 camera;
	float zoomScale = 1;
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
			else if (e.type == SDL_KEYDOWN && e.key.repeat >= 0)
			{
				if (e.key.keysym.sym == SDLK_p)
				{
					zoomScale += 0.02;
					SDL_RenderSetScale(renderer, zoomScale, zoomScale);
				}
				else if (e.key.keysym.sym == SDLK_o)
				{
					zoomScale -= 0.02;
					SDL_RenderSetScale(renderer, zoomScale, zoomScale);
				}
			}
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_LEFT)
				{
					GetData(e1, Kinematics)->acc.x -= v;
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					GetData(e1, Kinematics)->acc.x += v;
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					GetData(e1, Kinematics)->acc.y -= v;
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					GetData(e1, Kinematics)->acc.y += v;
					keyPressed = true;
				}
			}
			else if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_LEFT)
				{
					GetData(e1, Kinematics)->acc.x -= -v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					GetData(e1, Kinematics)->acc.x += -v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					GetData(e1, Kinematics)->acc.y -= -v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					GetData(e1, Kinematics)->acc.y += -v;
					keyPressed = false;
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
		//deltaTime *= 5;
		physics.UpdateTransform(deltaTime);
		physics.UpdateColliders();
		physics.FindSolveCollisions();

		// Camera updating
		//camera.x = (camera.Get<Transform>(camera.focusedEntity).x - window.w/2)  * zoomScale;
		//camera.y = (camera.Get<Transform>(camera.focusedEntity).x - window.w/2)  * zoomScale;
		camera.x = GetData(e1, Transform)->pos.x - (window.w / 2) / zoomScale;
		camera.y = GetData(e1, Transform)->pos.y - (window.h / 2) / zoomScale;

		std::cout << GetData(e1, Kinematics)->vel.x << " " << GetData(e1, Kinematics)->vel.y << '\n';
		//camera = { 0,0 };

		RenderSystem::Render(camera);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		/*auto& shape = *GetData(e1, Collision)->collider;
		for (int i = 0; i < shape.vertices.size(); i++)
		{
			if (i + 1 == shape.vertices.size())
			{
				SDL_RenderDrawLine(renderer, shape.vertices[i].x - camera.x, shape.vertices[i].y - camera.y, shape.vertices[0].x - camera.x, shape.vertices[0].y - camera.y);
				break;
			}
			SDL_RenderDrawLine(renderer, shape.vertices[i].x - camera.x, shape.vertices[i].y - camera.y, shape.vertices[i + 1].x - camera.x, shape.vertices[i + 1].y - camera.y);
		}*/


		//std::cout << (currentUpdate - lastUpdate) << " ms\n";
		++frame;
		if (SDL_GetTicks() % 1000 == 0 && frame > 5)
		{
			std::cout << frame << " fps\n";
			frame = 0;
		}


		lastUpdate = currentUpdate;
		//Update screen
		SDL_RenderPresent(renderer);


		//printf("fps: %f\n", 1/deltaTime);
		//printf("deltatime: %f\n", deltaTime);

		//if (1000 / 60 > deltaTime) SDL_Delay(1000 / 100 - deltaTime); 
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
