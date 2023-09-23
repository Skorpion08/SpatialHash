#pragma once

#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <iostream>

#include "TextureManager.h"
#include "Vector.h"
#include "Shape.h"
#include "Type.h"
#include "Column.h"

using EntityID = size_t;
using ComponentID = EntityID;
using ArchetypeID = EntityID;

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
inline std::unordered_map<Type, Archetype, TypeHash> typeToArchetype;


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
	inline ComponentID GetComponentID()
	{
		static ComponentID componentID = nextComponentID++;
		return componentID;
	}

	template <typename T, typename... Args>
	void Add(EntityID entityID, Args&&... args)
	{
		if (!entityRecord.contains(entityID))
			return;

		ComponentID componentID = GetComponentID<T>();


		EntityRecord& record = entityRecord[entityID];
		Archetype* oldArchetype = record.archetype;
		Type& oldType = oldArchetype->type;
		Type newType = oldArchetype->type;
		auto& oldTable = oldArchetype->table;
		newType.Insert(componentID);

		bool createNewArchetype = !typeToArchetype.contains(newType);

		Archetype* newArchetype = &typeToArchetype[newType];
		record.archetype = newArchetype;
		auto& table = newArchetype->table;
		size_t oldRow = record.row;
		int newCompIndex = newType.FindIndexFor(componentID);

		// Create a new archetype if we couldn't find one
		if (createNewArchetype)
		{
			table.reserve(newType.Size());
			newArchetype->type = newType;
			for (int i = 0; i < newType.Size(); ++i)
			{
				if (i == newCompIndex)
				{
					table.emplace_back(Column(typeid(T), sizeof(T)));
					continue;
				}
				table.emplace_back(Column(oldTable[i].type, oldTable[i].element_size));
			}
		}

		record.row = table[0].m_count;

		// Move over the data
		for (int i = 0; i < newType.Size(); ++i)
		{
			if (i == newCompIndex)
			{
				table[newCompIndex].Insert<T>(std::move(args)...);
				continue;
			}

			Column* oldCol = &oldTable[i];
			table[i].ResizeFor(1);
			memcpy(table[i].GetAddress(table[i].m_count++), oldCol->GetAddress(oldRow), table[i].element_size);
			//--oldCol->m_count;
			//memcpy(oldCol->Get<T>(oldRow), oldCol->Get<T>(m_count--), sizeof(T)); // We leave this out for now as we dont store wich row has wich entity. So now it simply leaves a hole in the array
		}
	}

	Sprite* AddSprite(EntityID entityID, SDL_Rect* src, SDL_Rect* dst, Texture* texture);
	Transform* AddTransform(EntityID entityID, Vector2 position = { 0,0 }, float rotation = 0.0f, Vector2 scale = { 0,0 });
	Kinematics* AddKinematics(EntityID entityID, Vector2 vel = { 0,0 }, Vector2 acc = { 0,0 }, float angularVel = 0.0f, float angularAcc = 0.0f);
	Collision* AddCollision(EntityID entityID, Shape2D* collider, bool blockCollision = true);
	Rigidbody* AddRigidbody(EntityID entityID, float mass, bool enableGravity, float gravityAcc = 9.81, float elasticity = 0.6, float staticFriction = 0.0, float dynamicFriction = 0.0);


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

template<typename T, typename ...Args>
inline T* Column::Insert(Args && ...args)
{
	if (typeid(T) == type)
	{
		if (elements.size() + element_size > elements.capacity())
		{
			elements.reserve(elements.capacity() * 1.5);
		}
		elements.resize(elements.size() + element_size);
		T* elementPtr = reinterpret_cast<T*>(elements.data() + m_count++ * element_size);
		*elementPtr = T(std::forward<Args>(args)...);
		return elementPtr;
	}
	return nullptr;
}

template<typename T>
inline T* Column::Get(size_t row)
{
	if (typeid(T) == type)
	{
		return reinterpret_cast<T*>(elements.data() + row * element_size);
	}
	return nullptr;
}