#pragma once

#include <SDL.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>
#include <typeindex>

#include <map>
#include <set>

#include "TextureManager.h"
#include "Vector.h"
#include "Shape.h"

using EntityID = size_t;
using ComponentID = EntityID;
using ArchetypeID = EntityID;

//using Type = std::vector<ComponentID>;
using Type = std::set<ComponentID>;

enum EntityMovability
{
	Dynamic,
	Static
};

struct Entity
{
	EntityID id;
	bool active;
	EntityMovability movability;

	template<typename T>
	void Add()
	{

	}

	template<typename T>
	void Get()
	{

	}
};

// Components:
struct Column
{
	Column(const std::type_index& t, size_t size) : type(t), element_size(size), m_count(0) {}

	std::vector<uint8_t> elements; // We use a uint8_t as a buffer for memory
	const std::type_index& type;
	size_t element_size;
	size_t m_count;
	
	template <typename T, typename... Args>
	bool Insert(Args&&... args)
	{
		if (typeid(T) == type)
		{
			elements.resize(elements.size() + element_size);
			T* elementPtr = reinterpret_cast<T*>(elements.data() + m_count++*element_size);
			*elementPtr = T(std::forward<Args>(args)...);
			printf("\t%d %d\n", m_count, elements.size());
			return true;
		}
		return false;
	}

	template <typename T>
	T* Get(size_t row)
	{
		if (typeid(T) == type)
		{
			return reinterpret_cast<T*>(elements.data() + row * element_size);
		}
		return nullptr;
	}
};
struct Archetype
{
	ArchetypeID id;
	Type type;
	std::vector<Column> table;
};

struct EntityRecord
{
	Archetype* archetype;
	size_t row;
};

inline std::unordered_map<EntityID, EntityRecord> entityRecord;
inline std::map<Type, Archetype> typeToArchetype;

using ArchetypeSet = std::unordered_set<ArchetypeID>;


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
	Vector2 scale;
};

struct Kinematics
{
	Vector2 vel;
	Vector2 acc;

	float angularVel;
	float angularAcc;
};

struct Rigidbody // All forces are in N
{
	float mass;

	bool enableGravity;
	float gravityAcc;

	float elasticity;

	float staticFriction;
	float dynamicFriction;

	Vector2 resultantForce;
	Vector2 constForces;
	//float torque;
	//float momentOfInertia;
};

struct Collision
{
	Shape2D* collider;
	bool blockCollision;
};

struct World
{
	//std::vector<>
};


// Holds what components entities use
struct Registry
{
	std::unordered_map<EntityID, Sprite> sprites;
	std::unordered_map<EntityID, Transform> transforms;
	std::unordered_map<EntityID, Kinematics> kinematics;
	std::unordered_map<EntityID, Collision> collisionComponents;
	std::unordered_map<EntityID, Rigidbody> rigidbodies;
};
class ECS
{
	ECS() : nextEntityID(0) {}

public:
	// Prevent copying
	ECS(ECS const&) = delete;
	ECS& operator=(ECS const&) = delete;


	static ECS& GetInstance()
	{
		static ECS instance;
		return instance;
	}

	// Returns its ID
	EntityID CreateEnitity(EntityMovability movability = Dynamic);

	template <typename T>
	ComponentID GetComponentID()
	{
		static ComponentID componentID = nextComponentID++;
		return componentID;
	}

	template <typename T, typename... Args>
	T* Add(EntityID entityID, Args&&... args)
	{
		ComponentID componentID = GetComponentID<T>();

		if (!entityRecord.contains(entityID))
			return nullptr;

		EntityRecord& record = entityRecord[entityID];
		Type type = record.archetype->type;
		type.insert(componentID);

		if (typeToArchetype.contains(type))
		{

		}

		// ignore rest of the code in this function

		registry.sprites.emplace(entityID, T(std::forward<Args>(args)...));
		return &GetInstance().GetRegistry().sprites[entityID];
	}

	Sprite* AddSprite(EntityID entityID, SDL_Rect* src, SDL_Rect* dst, Texture* texture);
	Transform* AddTransform(EntityID entityID, Vector2 position = { 0,0 }, float rotation = 0.0f, Vector2 scale = { 0,0 });
	Kinematics* AddKinematics(EntityID entityID, Vector2 vel = { 0,0 }, Vector2 acc = { 0,0 }, float angularVel = 0.0f, float angularAcc = 0.0f);
	Collision* AddCollision(EntityID entityID, Shape2D* collider, bool blockCollision = true);
	Rigidbody* AddRigidbody(EntityID entityID, float mass, bool enableGravity, float gravityAcc = 9.81, float elasticity = 0.6, float staticFriction = 0.4, float dynamicFriction = 0.3);


	void DestroyEntity(EntityID entityID);

	inline bool EntityValid(EntityID entityID) { return entityID >= 0 && entityID < NumberOfEntities() && GetEntities()[entityID].active;}

	inline size_t NumberOfEntities() { return entities.size(); }

	inline std::vector<Entity>& GetEntities() { return entities; }

	inline Registry& GetRegistry() { return registry; }

private:
	EntityID nextEntityID;
	ComponentID nextComponentID;
	
	std::vector<Entity> entities;
	Registry registry;
};

// Systems:

struct SpriteSystem
{
	// Update all sprites
	void Update(Vector2 cameraPos = { 0,0 });

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
	CollisionInfo POLYtoPOLY(Shape2D& shapeA, Shape2D& shapeB);

	//CollisionInfo& CIRCLEtoCIRCLE(Circle& a, Circle& b);
	//CollisionInfo& CIRCLEtoCIRCLE(Circle& a, Circle& b);
	void SolveCollisions();
};