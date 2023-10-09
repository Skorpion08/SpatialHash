#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <vector>
#include <stdexcept>
#include <iostream>

#include "Window.h"
#include "TextureManager.h"
#include "ECS.h"
#include "Log.h"
//#include "EntityManager.h"


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

	double deltaTime;

	// ecs stuff
	//ECS& ecs = ECS::GetInstance();
	TextureManager& textureManager = TextureManager::GetInstance();

	//SpriteSystem spritesSystem;
	//TransformSystem transformSystem;
	//CollisionSystem collisionSystem;
};