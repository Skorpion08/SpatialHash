#pragma once

#include <SDL.h>

#include <vector>
#include <unordered_map>

#include "TextureManager.h"
#include "Vector.h"

using EntityID = size_t;

// Entity is a bool to indicate whether it is active or not
using Entity = bool;

// Components:

struct SpriteComponent
{
	SDL_Rect* src;
	SDL_Rect* dst;
	Texture* texture;
};

struct TransformComponent
{
	float posX, posY;
	float velX, velY;
	float accX, accY;
};

struct CollisionComponent
{

};

struct AABB
{
	SDL_Rect* boundingBox;
};

struct SAT
{

};


// Holds what components entities use
struct Registry
{
	std::unordered_map<EntityID, SpriteComponent> sprites;
	std::unordered_map<EntityID, TransformComponent> transforms;
	std::unordered_map<EntityID, AABB> aabbs;
	std::unordered_map<EntityID, SAT> sats;
};
/*
struct Entity
{
	// All possible components here
	bool destroyed = false;
};
*/
class EntityManager
{
	EntityManager() : nextEntityID(0) {}

public:
	// Prevent copying
	EntityManager(EntityManager const&) = delete;
	EntityManager& operator=(EntityManager const&) = delete;


	static EntityManager& GetInstance()
	{
		static EntityManager instance;
		return instance;
	}

	// Returns its ID
	EntityID CreateEnitity();

	void DestroyEntity(EntityID entityID);

	inline bool IsEntityIDValid(EntityID entityID) { return entityID > 0 && entityID < NumberOfEntities();}

	inline size_t NumberOfEntities() { return entities.size(); }

	inline std::vector<Entity>& GetEntities() { return entities; }

	inline Registry& GetRegistry() { return registry; }

private:
	EntityID nextEntityID;
	//std::vector<Entity> entities;
	std::vector<Entity> entities;

	Registry registry;
};

// Systems:

struct SpriteSystem
{
	// Update all sprites
	void Update();

	void Render();
};

struct TransformSystem
{
	// Applies velocity
	void Update(double deltaTime);
};

struct CollisionInfo
{
	bool overlap;
	Vector2 mtv;
};

struct CollisionSystem
{
	void Update();

	// Takes in two aabbs
	CollisionInfo IsCollidingAABB(SDL_Rect& a, SDL_Rect& b);
	bool IsCollidingSAT(std::vector<Point2>& verticesA, std::vector<Point2>& verticesB);
	void SolveCollisions();
};