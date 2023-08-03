#pragma once

#include <SDL.h>

#include <vector>
#include <unordered_map>

#include "TextureManager.h"
#include "Vector.h"
#include "Shape.h"

using EntityID = size_t;

// Entity is a bool to indicate whether it is active or not
using Entity = bool;

// Components:

struct Sprite
{
	SDL_Rect* src;
	SDL_Rect* dst;
	Texture* texture;
};

struct Transform
{
	Transform() = default;
	Transform(float posX, float posY) : pos(posX, posY) {}
	Transform(Vector2 _pos, float _rotation, Vector2 _scale) : pos(_pos), rotation(_rotation), scale(_scale) {}
	Vector2 pos;
	float rotation;
	Vector2 scale = { 1.0f,1.0f };
};

struct Kinematics
{
	Vector2 vel;
	Vector2 acc;

	float angularVel;
	float angularAcc;
};

struct Rigidbody
{
	float torque;
	float momentOfInertia;
};

struct Collision
{
	bool blockCollision;
	Shape2D* collider;
};

// Holds what components entities use
struct Registry
{
	std::unordered_map<EntityID, Sprite> sprites;
	std::unordered_map<EntityID, Transform> transforms;
	std::unordered_map<EntityID, Kinematics> kinematics;
	std::unordered_map<EntityID, Collision> collisionComponents;
};
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

	Sprite* AddSprite(EntityID entityID, SDL_Rect* src, SDL_Rect* dst, Texture* texture);
	Transform* AddTransform(EntityID entityID, Vector2 position = { 0,0 }, float rotation = 0.0f, Vector2 scale = { 0,0 });
	Kinematics* AddKinematics(EntityID entityID, Vector2 vel = { 0,0 }, Vector2 acc = { 0,0 }, float angularVel = 0.0f, float angularAcc = 0.0f);
	Rigidbody* AddRigidbody(EntityID entityID);
	Collision* AddCollision(EntityID entityID, Shape2D* collider, bool blockCollision = true);


	void DestroyEntity(EntityID entityID);

	inline bool EntityIDValid(EntityID entityID) { return entityID >= 0 && entityID < NumberOfEntities();}

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

	void SetScale(EntityID entityID, Vector2 newScale);
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
	CollisionInfo AABBtoAABB(AABB& a, AABB& b);
	CollisionInfo OBBtoOBB(Shape2D& shapeA, Shape2D& shapeB);
	CollisionInfo POLYtoPOLY(Shape2D& shapeA, Shape2D& shapeB);

	//CollisionInfo& CIRCLEtoCIRCLE(Circle& a, Circle& b);
	//CollisionInfo& CIRCLEtoCIRCLE(Circle& a, Circle& b);
	void SolveCollisions();
};