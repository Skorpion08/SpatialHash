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


	EntityID fence = ecs.CreateEnitity();
	ecs.AddTransform(fence);
	SDL_Rect rectum = { 0,0, 4176 * 2, 960 * 2 };
	ecs.AddSprite(fence, nullptr, &rectum, textureManager.Load("resources/fence.png"));


	EntityID square2 = ecs.CreateEnitity();
	ecs.AddTransform(square2, { 600,600 });
	ecs.AddKinematics(square2);
	SDL_Rect dst2(100, 100, 100, 100);
	ecs.AddSprite(square2, nullptr, &dst2, textureManager.Load("resources/square.png"));
	OBB obb2(100, 100);
	
	ecs.AddRigidbody(square2, 60, true, 490, 1);
	ecs.AddCollision(square2, &obb2);

	EntityID square3 = ecs.CreateEnitity();
	ecs.AddTransform(square3, { 400,600 });
	ecs.AddKinematics(square3);
	SDL_Rect dst3(100, 100, 100, 100);
	ecs.AddSprite(square3, nullptr, &dst3, textureManager.Load("resources/square.png"));
	AABB aabb1(100, 100);

	ecs.AddRigidbody(square3, 50, true, 490);

	ecs.AddCollision(square3, &aabb1);
	/*
	EntityID swastyka = ecs.CreateEnitity();
	ecs.AddTransform(swastyka);
	ecs.AddKinematics(swastyka, { 0,0 }, { 0,0 }, 0, 1);
	SDL_Rect dsts(0, 0, 400, 400);
	ecs.AddSprite(swastyka, nullptr, &dsts, textureManager.Load("resources/swastyka.png"));

	OBB swastik(400, 400);
	ecs.AddCollision(swastyka, &swastik);
	*/
	EntityID rectangle = ecs.CreateEnitity(Static);
	ecs.AddTransform(rectangle, { 400,700 });
	ecs.AddKinematics(rectangle);
	SDL_Rect dstrect(0, 0, 100000, 100);
	ecs.AddSprite(rectangle, nullptr, &dstrect, textureManager.Load("resources/square.png"));
	AABB aabb2(100000, 100);

	ecs.AddRigidbody(rectangle, 0, false, 0, 1);
	ecs.AddCollision(rectangle, &aabb2);

	/*
	EntityID rectangle2 = ecs.CreateEnitity(Dynamic);
	ecs.AddTransform(rectangle2, { 400, 0});
	ecs.AddKinematics(rectangle2, {0,100});
	SDL_Rect dstrect2(0, 0, 10000, 100);
	ecs.AddSprite(rectangle2, nullptr, &dstrect2, textureManager.Load("resources/square.png"));
	AABB aabb3(100000, 100);

	ecs.AddRigidbody(rectangle2, 1e6, false);
	ecs.AddCollision(rectangle2, &aabb3);
	*/
	/*
	EntityID square4 = ecs.CreateEnitity();
	ecs.AddTransform(square4, { 400,400 });
	SDL_Rect dst4(100, 100, 100, 100);
	ecs.AddSprite(square4, nullptr, &dst4, textureManager.Load("resources/triangle.png"));
	AABB aabb2(100, 100, { 400,400 });

	ecs.GetRegistry().collisionComponents[square4].collider = &aabb2;

	EntityID square5 = ecs.CreateEnitity();
	ecs.AddTransform(square5, { 400,400 });
	SDL_Rect dst5(100, 100, 100, 100);
	ecs.AddSprite(square5, nullptr, &dst5, textureManager.Load("resources/triangle.png"));
	AABB aabb3(100, 100, { 400,400 });

	ecs.GetRegistry().collisionComponents[square5].collider = &aabb3;
	*/

	EntityID border = ecs.CreateEnitity(Static);
	ecs.AddTransform(border, { 50000, 700 });
	SDL_Rect bordeah = { 0,0,100,10000 };
	ecs.AddSprite(border, nullptr, &bordeah, textureManager.Load("resources/square.png"));
	//ecs.Add<Sprite>(border, nullptr, &bordeah, textureManager.Load("resources/square.png"));
	AABB borderaa(100, 10000);
	ecs.AddCollision(border, &borderaa);
	ecs.AddRigidbody(border, 0, false);


	//Archetype arch;
	//arch.table.push_back(Column(typeid(Transform), sizeof(Transform)));
	//arch.table.push_back(Column(typeid(Sprite), sizeof(Sprite)));
	//arch.table.push_back(Column(typeid(Kinematics), sizeof(Kinematics)));

	//arch.table[0].Insert<Transform>();
	//arch.table[0].Insert<Transform>();
	//arch.table[0].Insert<Transform>();
	//arch.table[0].Insert<Transform>();
	////arch.table[0].Insert<Transform>(Transform());

	//arch.table[1].Insert<Sprite>();
	//arch.table[1].Insert<Sprite>();
	//arch.table[1].Insert<Sprite>();

	//arch.table[2].Insert<Kinematics>();
	//arch.table[2].Insert<Kinematics>();

	//arch.table[2].Get<Kinematics>(0)->vel.x = 10;
	//printf("\t\t\n %d\n", sizeof(arch.table[0].elements[0]) * arch.table[0].elements.size() + sizeof(arch.table[1].elements[0]) * arch.table[1].elements.size() + sizeof(arch.table[2].elements[0]) * arch.table[2].elements.size());

	//std::cout << ecs.GetComponentID<Transform>() << " " << ecs.GetComponentID<Transform>() << " " << ecs.GetComponentID<Kinematics>() << " " << ecs.GetComponentID<Sprite>() << "\n";
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
		EntityID ent = ecs.CreateEnitity();
		ecs.AddTransform(ent, { (float)10*i, 0 });
		ecs.AddKinematics(ent);
		ecs.AddSprite(ent, nullptr, &s[i], textureManager.Load("resources/square.png"));
		ecs.AddCollision(ent, &c[i]);
		ecs.AddRigidbody(ent, 100, true, 100, 0.4);
		printf("%f\n", (i * 100) / 1e6);

	}
	*/

	EntityID testy = ecs.CreateEnitity();
	EntityID testy2 = ecs.CreateEnitity();
	EntityID dwa = ecs.CreateEnitity();

	ecs.Add<Transform>(testy, 10, 20);
	ecs.Add<Transform>(testy2, 20, 50);
	ecs.Add<Transform>(dwa, 20, 50);

	struct A {
		int a;
		int b;
		int c;
	};

	auto start = std::chrono::high_resolution_clock::now();
	// 4000 = 1 ms na moim sprzecie
	for (int i = 0; i < 4000*1; ++i)
	{
		ecs.Add<Transform>(ecs.CreateEnitity(), i*5, -i*7);
		/*ecs.Add<Kinematics>(ecs.CreateEnitity(), Vector2{(float)i * 1, (float)-i * 7 }, Vector2{ (float)i*2, (float)-i*5}, 0, 0);
		ecs.Add<A>(ecs.CreateEnitity(), i * 5, -i * 7, i*i);*/
	}
		
	auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> duration = end - start; std::cout << "It took: " << duration.count() * 1000 << " ms\n";


	start = std::chrono::high_resolution_clock::now();
	// 4000 = 1 ms na moim sprzecie
	for (int i = 0; i < 4000 * 10; ++i)
	{
		ecs.Add<Transform>(ecs.CreateEnitity(), i * 5, -i * 7);
		/*ecs.Add<Kinematics>(ecs.CreateEnitity(), Vector2{(float)i * 1, (float)-i * 7 }, Vector2{ (float)i*2, (float)-i*5}, 0, 0);
		ecs.Add<A>(ecs.CreateEnitity(), i * 5, -i * 7, i*i);*/
	}

	end = std::chrono::high_resolution_clock::now(); duration = end - start; std::cout << "It took: " << duration.count() * 1000 << " ms\n";

	start = std::chrono::high_resolution_clock::now();
	// 4000 = 1 ms na moim sprzecie
	for (int i = 0; i < 4000 * 100; ++i)
	{
		ecs.Add<Transform>(ecs.CreateEnitity(), i * 5, -i * 7);
		/*ecs.Add<Kinematics>(ecs.CreateEnitity(), Vector2{(float)i * 1, (float)-i * 7 }, Vector2{ (float)i*2, (float)-i*5}, 0, 0);
		ecs.Add<A>(ecs.CreateEnitity(), i * 5, -i * 7, i*i);*/
	}

	end = std::chrono::high_resolution_clock::now(); duration = end - start; std::cout << "It took: " << duration.count() * 1000 << " ms\n";

	start = std::chrono::high_resolution_clock::now();
	// 4000 = 1 ms na moim sprzecie
	for (int i = 0; i < 4000 * 500; ++i)
	{
		ecs.Add<Transform>(ecs.CreateEnitity(), i * 5, -i * 7);
		/*ecs.Add<Kinematics>(ecs.CreateEnitity(), Vector2{(float)i * 1, (float)-i * 7 }, Vector2{ (float)i*2, (float)-i*5}, 0, 0);
		ecs.Add<A>(ecs.CreateEnitity(), i * 5, -i * 7, i*i);*/
	}

	end = std::chrono::high_resolution_clock::now(); duration = end - start; std::cout << "It took: " << duration.count() * 1000 << " ms\n";

	start = std::chrono::high_resolution_clock::now();
	// 4000 = 1 ms na moim sprzecie
	for (int i = 0; i < 4000 * 1000; ++i)
	{
		ecs.Add<Transform>(ecs.CreateEnitity(), i * 5, -i * 7);
		/*ecs.Add<Kinematics>(ecs.CreateEnitity(), Vector2{(float)i * 1, (float)-i * 7 }, Vector2{ (float)i*2, (float)-i*5}, 0, 0);
		ecs.Add<A>(ecs.CreateEnitity(), i * 5, -i * 7, i*i);*/
	}

	end = std::chrono::high_resolution_clock::now();duration = end - start; std::cout << "It took: " << duration.count() * 1000 << " ms\n";

	start = std::chrono::high_resolution_clock::now();
	// 4000 = 1 ms na moim sprzecie
	for (int i = 0; i < 4000 * 4000; ++i)
	{
		ecs.Add<Transform>(ecs.CreateEnitity(), i * 5, -i * 7);
		/*ecs.Add<Kinematics>(ecs.CreateEnitity(), Vector2{(float)i * 1, (float)-i * 7 }, Vector2{ (float)i*2, (float)-i*5}, 0, 0);
		ecs.Add<A>(ecs.CreateEnitity(), i * 5, -i * 7, i*i);*/
	}

	end = std::chrono::high_resolution_clock::now(); duration = end - start; std::cout << "It took: " << duration.count() * 1000 << " ms\n";

	std::cout << "wartosci\n";

	Archetype& arch = typeToArchetype[{ecs.GetComponentID<Transform>()}];
	Archetype& arch2 = typeToArchetype[{ecs.GetComponentID<Kinematics>()}];
	Archetype& arch3 = typeToArchetype[{ecs.GetComponentID<A>()}];

	/*std::cout << arch.table[0].Get<Transform>(0)->pos.x << '\n';
	std::cout << arch.table[0].Get<Transform>(1)->pos.x << '\n';
	std::cout << arch.table[0].Get<Transform>(1000)->pos.x << '\n';
	std::cout << arch2.table[0].Get<Kinematics>(999)->vel.x << '\n';
	std::cout << arch3.table[0].Get<A>(1332)->c << '\n';*/
	//std::cout << arch.table[0].Get<Transform>(0)->pos.x;

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
					ecs.GetRegistry().kinematics[square2].acc.x += -v;
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					ecs.GetRegistry().kinematics[square2].acc.x += v;
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					ecs.GetRegistry().kinematics[square2].acc.y += -v;
					SDL_RenderSetScale(renderer, 1, 1);
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					ecs.GetRegistry().kinematics[square2].acc.y += v;
					SDL_RenderSetScale(renderer, 0.5, 0.5);
					keyPressed = true;
				}
				else if (e.key.keysym.sym == SDLK_o)
				{
					ecs.GetRegistry().kinematics[square2].angularVel = -b;
				}
				else if (e.key.keysym.sym == SDLK_p)
				{
					ecs.GetRegistry().kinematics[square2].angularVel = b;
				}
			}
			else if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_LEFT)
				{
					ecs.GetRegistry().kinematics[square2].acc.x -= -v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					ecs.GetRegistry().kinematics[square2].acc.x -= v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_UP)
				{
					ecs.GetRegistry().kinematics[square2].acc.y -= -v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					ecs.GetRegistry().kinematics[square2].acc.y -= v;
					keyPressed = false;
				}
				else if (e.key.keysym.sym == SDLK_o)
				{
					ecs.GetRegistry().kinematics[square2].angularVel = 0;
				}
				else if (e.key.keysym.sym == SDLK_p)
				{
					ecs.GetRegistry().kinematics[square2].angularVel = 0;
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
		transformSystem.Update(deltaTime);
		collisionSystem.Update();
		collisionSystem.SolveCollisions();

		// Camera updating
		camera.x = ecs.GetRegistry().transforms[square2].pos.x - window.w / 2;
		camera.y = ecs.GetRegistry().transforms[square2].pos.y - window.h / 2;

		//ecs.GetRegistry().transforms[rectangle].pos = { 400, 700 };
		//ecs.GetRegistry().kinematics[rectangle].vel = { 0,0 };
		//printf("vel: %f\n", ecs.GetRegistry().kinematics[square2].vel.x);
		//printf("camera: %f %f\n", camera.x, camera.y);
		//printf("acc %f %f\n", ecs.GetRegistry().kinematics[square2].acc.x, ecs.GetRegistry().kinematics[square2].acc.y);
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

	//	if (1000 / 60 > deltaTime) SDL_Delay(1000 / 100 - deltaTime); 
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
