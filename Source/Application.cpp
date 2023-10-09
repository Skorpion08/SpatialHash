#include <cstdio>
#include <chrono>
#include <random>
#include "NewColumn.h"

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
		int a = 0;
		int b = 0;
		int c = 0;
		~A() { std::cout << "destroyed\n"; }
	};
	struct B{
		B() = default;
		B(int _x) : x(_x) { std::cout << "created\n"; }
		int x;
		~B() { std::cout << "destroyed\n"; }
	};
	struct C{ int x; };
	struct D{ int x; };
	struct E { int x; };
	struct F { int x, y, z; }; struct G { int x, y, z, w; }; struct H { int x, y; };

	std::random_device device;
	std::mt19937 mt(device());
	std::uniform_int_distribution<int> dist(-100, 100);

	std::vector<New::ColumnBase*> columns;
	columns.emplace_back(new New::Column<int>());
	New::Column<int>* Col = static_cast<New::Column<int>*>(columns[0]);
	Col->PushBack(2);
	Col->PushBack(3);
	std::cout << *Col->Get(0) << '\n';
	columns[0]->Destroy(0);
	std::cout << *Col->Get(0) << '\n';


	for (auto& col : columns)
		delete col;
	//COMPONENT(A);
	//COMPONENT(B);
	//TAG(Enemy);
	//
	//EntityID e1 = ECS::NewEnitity();
	//EntityID e2 = ECS::NewEnitity();

	////AddTag(e1, Enemy);
	//AddData(e1, A, 0, 0, 0);
	//AddData(e2, A, 1, 2, 3);
	//auto start = std::chrono::high_resolution_clock::now();
	//for (int i = 0; i < 4; ++i)
	//{
	//	EntityID e0 = ECS::NewEnitity();
	//	//AddData(e, A, i*2, i * 3, i * 4);
	//	AddData(e0, A, dist(mt), dist(mt), dist(mt));
	//	if (i % 4 == 0)
	//	{
	//		AddTag(e0, Enemy);
	//	}
	//	if (i % 16 == 0)
	//	{
	//		AddType(e0, B);
	//	}
	//}
	//auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> duration = end - start; std::cout << "Setup took: " << duration.count() * 1000 << " ms\n";
	//start = std::chrono::high_resolution_clock::now();
	//auto query = ECS::Query({ A_ID });
	//end = std::chrono::high_resolution_clock::now(); duration = end - start; std::cout << "Querying took: " << duration.count() * 1000 << " ms\n";
	//start = std::chrono::high_resolution_clock::now();
	//int sum = 0;
	//for (int i = 0; i < query.size(); ++i)
	//{
	//	A* aa = query[i]->columns[query[i]->type.FindIndexFor(getID(A))].Get<A>(0);
	//	for (int j = 0; j < query[i]->columns[0].m_count; ++j)
	//	{
	//		sum += aa[j].a;
	//		sum -= aa[j].b;
	//		sum += aa[j].c;
	//	}
	//}

	//end = std::chrono::high_resolution_clock::now(); duration = end - start; std::cout << "Processing took: " << duration.count() * 1000 << " ms\n";
	//std::cout << sum << '\n';

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
	//std::random_device device;
	//std::mt19937 mt(device());
	//std::uniform_int_distribution<int> dist(-1000, 1000);
	//std::ios::sync_with_stdio(false);
	//int is = 1000;
	//for (int i = 0; i < is; ++i)
	//{
	//	EntityID e = ecs.CreateEnitity();
	//	ecs.Add<A>(e, dist(mt), dist(mt), dist(mt));
	//	ecs.Add<B>(e, dist(mt));
	//	ecs.Add<C>(e, dist(mt));
	//	ecs.Add<D>(e, dist(mt));
	//}
	//Archetype* arch = ecs.QueryExact<A, B, C, D>();
	//A* aa = arch->table[0].Get<A>(0);
	//B* bb = arch->table[1].Get<B>(0);
	//C* cc = arch->table[2].Get<C>(0);
	//D* dd = arch->table[3].Get<D>(0);
	//auto start = std::chrono::high_resolution_clock::now();
	//int sum = 0;
	//for (int e = 0; e < is; ++e)
	//{
	//	sum += aa[e].a;
	//	sum *= bb[e].x;
	//	sum += cc[e].x;
	//	sum *= dd[e].x;
	//}
	//auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> duration = end - start; std::cout << "Processing took: " << duration.count() * 1000 << " ms\n";
	//std::cout << sum;
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
		//transformSystem.Update(deltaTime);
		//collisionSystem.Update();
		//collisionSystem.SolveCollisions();

		// Camera updating
		// camera.x = (camera.Get<Transform>(camera.focusedEntity).x - window.w/2)  * zoomScale;
		// camera.y = (camera.Get<Transform>(camera.focusedEntity).x - window.w/2)  * zoomScale;
		//camera.x = ecs.GetRegistry().transforms[square2].pos.x - window.w / 2;
		//camera.y = ecs.GetRegistry().transforms[square2].pos.y - window.h / 2;
		camera.x = 0;
		camera.y = 0;

		//ecs.GetRegistry().transforms[rectangle].pos = { 400, 700 };
		//ecs.GetRegistry().kinematics[rectangle].vel = { 0,0 };
		//printf("vel: %f\n", ecs.GetRegistry().kinematics[square2].vel.x);
		//printf("camera: %f %f\n", camera.x, camera.y);
		//printf("acc %f %f\n", ecs.GetRegistry().kinematics[square2].acc.x, ecs.GetRegistry().kinematics[square2].acc.y);

		//spritesSystem.Update(camera);
		//spritesSystem.Render();

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
