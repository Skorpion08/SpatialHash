#include <cstdio>
#include <chrono>

#include "Application.h"
#include "Vector.h"
#include "Shape.h"


Application::Application()
{
	window.w = 1000;
	window.h = 700;
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

#if 0
	EntityID fence = ecs.CreateEnitity();
	ecs.AddTransform(fence);
	SDL_Rect rectum = { 0,0, 4176 * 2, 960 * 2 };
	ecs.AddSprite(fence, nullptr, &rectum, textureManager.Load("resources/fence.png"));


	EntityID square2 = ecs.CreateEnitity();
	ecs.AddTransform(square2, { 700,400 });
	ecs.AddKinematics(square2);
	SDL_Rect dst2(100, 100, 100, 100);
	ecs.AddSprite(square2, nullptr, &dst2, textureManager.Load("resources/square.png"));
	OBB obb2(100, 100);
	
	ecs.AddRigidbody(square2, 100, true, 490, 0.8);
	ecs.AddCollision(square2, &obb2);

	EntityID square3 = ecs.CreateEnitity();
	ecs.AddTransform(square3, { 300,400 });
	ecs.AddKinematics(square3);
	SDL_Rect dst3(100, 100, 100, 100);
	ecs.AddSprite(square3, nullptr, &dst3, textureManager.Load("resources/square.png"));
	AABB aabb1(100, 100);

	ecs.AddRigidbody(square3, 60, true, 490, 0.8);

	ecs.AddCollision(square3, &aabb1);

	EntityID square = ecs.CreateEnitity();
	ecs.AddTransform(square, { 500,400 });
	ecs.AddKinematics(square);
	SDL_Rect dst(100, 100, 100, 100);
	ecs.AddSprite(square, nullptr, &dst, textureManager.Load("resources/square.png"));
	AABB aabb(100, 100);

	ecs.AddRigidbody(square, 60, true, 490, 0.8);

	ecs.AddCollision(square, &aabb);
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

	EntityID rectangle2 = ecs.CreateEnitity(Static);
	ecs.AddTransform(rectangle2, { 400,0 });
	ecs.AddKinematics(rectangle2);
	SDL_Rect dstrect2(0, 0, 100000, 100);
	ecs.AddSprite(rectangle2, nullptr, &dstrect2, textureManager.Load("resources/square.png"));
	AABB aabb22(100000, 100);

	ecs.AddRigidbody(rectangle2, 0, false, 0, 1);
	ecs.AddCollision(rectangle2, &aabb22);

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
	ecs.AddTransform(border, { 1000, 700 });
	SDL_Rect bordeah = { 0,0,100,10000 };
	ecs.AddSprite(border, nullptr, &bordeah, textureManager.Load("resources/square.png"));
	//ecs.Add<Sprite>(border, nullptr, &bordeah, textureManager.Load("resources/square.png"));
	AABB borderaa(100, 10000);
	ecs.AddCollision(border, &borderaa);
	ecs.AddRigidbody(border, 0, false, 0, 1);

	EntityID border2 = ecs.CreateEnitity(Static);
	ecs.AddTransform(border2, { 0, 700 });
	SDL_Rect bordeah2 = { 0,0,100,10000 };
	ecs.AddSprite(border2, nullptr, &bordeah2, textureManager.Load("resources/square.png"));
	//ecs.Add<Sprite>(border, nullptr, &bordeah, textureManager.Load("resources/square.png"));
	AABB borderaa2(100, 10000);
	ecs.AddCollision(border2, &borderaa2);
	ecs.AddRigidbody(border2, 0, false, 0, 1);
#endif
	struct A {
		int a;
		int b;
		int c;
	};
	struct Tag {

	};
	EntityID e1 = ecs.CreateEnitity();
	EntityID e2 = ecs.CreateEnitity();
	EntityID e3 = ecs.CreateEnitity();
	ecs.Add<A>(e1);
	if(ecs.Get<Tag>(e1) != nullptr)
		std::cout << ecs.Get<Tag>(e1) << '\n\t';

	std::cout << ecs.Get<A>(e1) << '\n';

	
	//unsigned int n = 4000;
	//auto start = std::chrono::high_resolution_clock::now();
	//for (int i = 0; i < n; ++i)
	//{
	//	EntityID e = ecs.CreateEnitity();
	//	ecs.Add<A>(e, i, i + 1, i + 2);
	//	ecs.Add<Transform>(e, -i*1.5, i * 2);
	//}
	//auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> duration = end - start; std::cout << "Create took: " << duration.count() * 1000 << " ms\n";

	//start = std::chrono::high_resolution_clock::now();
	//int sum = 0;
	//A* firsta = ecs.Get<A>(4);
	//Transform* firstt = ecs.Get<Transform>(4);

	//for (int i = 0; i < n; ++i)
	//{
	//	sum += (firsta + i)->a;
	//	sum *= (firstt + 1)->pos.x;
	//}

	//end = std::chrono::high_resolution_clock::now(); duration = end - start; std::cout << "Get took: " << duration.count() * 1000 << " ms\n";
	//std::cout << "a sum: " << sum << '\n';
	//const int t = 4000000;
	//auto start = std::chrono::high_resolution_clock::now();
	////// 4000 = 1 ms na moim sprzecie
	//for (int i = 0; i < t; ++i)
	//{
	//	EntityID e = ecs.CreateEnitity();
	//	ecs.Add<Transform>(e, i*5, -i*7);
	//	//ecs.Add<A>(e, i * 5, -i * 7, i*i);
	//	//ecs.Add<Kinematics>(e, Vector2{(float)i * 1, (float)-i * 7 }, Vector2{ (float)i*2, (float)-i*5}, 0, 0);
	//}
	//	
	//auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> duration = end - start; std::cout << "It took: " << duration.count() * 1000 << " ms\n";

	//std::cout << "wartosci\n";

	//std::cout << arch.table[0].Get<Transform>(0)->pos.x << '\n';
	//std::cout << arch.table[0].Get<Transform>(1)->pos.x << '\n';
	//std::cout << arch22.table[0].Get<Transform>(0)->pos.y << '\n';
	//std::cout << arch22.table[1].Get<A>(0)->c << '\n';
	//std::cout << arch.table[0].Get<Transform>(1000)->pos.x << '\n';
	//std::cout << arch2.table[0].Get<Kinematics>(999)->vel.x << '\n';
	//std::cout << arch3.table[0].Get<A>(1332)->c << '\n';
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
			//else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			//{
			//	if (e.key.keysym.sym == SDLK_LEFT)
			//	{
			//		ecs.GetRegistry().kinematics[square2].acc.x += -v;
			//		keyPressed = true;
			//	}
			//	else if (e.key.keysym.sym == SDLK_RIGHT)
			//	{
			//		ecs.GetRegistry().kinematics[square2].acc.x += v;
			//		keyPressed = true;
			//	}
			//	else if (e.key.keysym.sym == SDLK_UP)
			//	{
			//		ecs.GetRegistry().kinematics[square2].acc.y += -v;
			//		SDL_RenderSetScale(renderer, 1, 1);
			//		keyPressed = true;
			//	}
			//	else if (e.key.keysym.sym == SDLK_DOWN)
			//	{
			//		ecs.GetRegistry().kinematics[square2].acc.y += v;
			//		//SDL_RenderSetScale(renderer, scaleX, scaleY);
			//		keyPressed = true;
			//	}
			//	else if (e.key.keysym.sym == SDLK_o)
			//	{
			//		ecs.GetRegistry().kinematics[square2].angularVel = -b;
			//	}
			//	else if (e.key.keysym.sym == SDLK_p)
			//	{
			//		ecs.GetRegistry().kinematics[square2].angularVel = b;
			//	}
			//}
			//else if (e.type == SDL_KEYUP && e.key.repeat == 0)
			//{
			//	if (e.key.keysym.sym == SDLK_LEFT)
			//	{
			//		ecs.GetRegistry().kinematics[square2].acc.x -= -v;
			//		keyPressed = false;
			//	}
			//	else if (e.key.keysym.sym == SDLK_RIGHT)
			//	{
			//		ecs.GetRegistry().kinematics[square2].acc.x -= v;
			//		keyPressed = false;
			//	}
			//	else if (e.key.keysym.sym == SDLK_UP)
			//	{
			//		ecs.GetRegistry().kinematics[square2].acc.y -= -v;
			//		keyPressed = false;
			//	}
			//	else if (e.key.keysym.sym == SDLK_DOWN)
			//	{
			//		ecs.GetRegistry().kinematics[square2].acc.y -= v;
			//		keyPressed = false;
			//	}
			//	else if (e.key.keysym.sym == SDLK_o)
			//	{
			//		ecs.GetRegistry().kinematics[square2].angularVel = 0;
			//	}
			//	else if (e.key.keysym.sym == SDLK_p)
			//	{
			//		ecs.GetRegistry().kinematics[square2].angularVel = 0;
			//	}
			//}
			
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
		//camera.x = ecs.GetRegistry().transforms[square2].pos.x - window.w / 2;
		//camera.y = ecs.GetRegistry().transforms[square2].pos.y - window.h / 2;
		camera.x = 0;
		camera.y = 0;

		//ecs.GetRegistry().transforms[rectangle].pos = { 400, 700 };
		//ecs.GetRegistry().kinematics[rectangle].vel = { 0,0 };
		//printf("vel: %f\n", ecs.GetRegistry().kinematics[square2].vel.x);
		//printf("camera: %f %f\n", camera.x, camera.y);
		//printf("acc %f %f\n", ecs.GetRegistry().kinematics[square2].acc.x, ecs.GetRegistry().kinematics[square2].acc.y);
		spritesSystem.Update({ static_cast<float>(camera.x), static_cast<float>(camera.y) });
		spritesSystem.Render();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		//SDL_RenderDrawLine(renderer, 0-camera.x, 350-camera.y, 1000-camera.x, 350-camera.y);
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
