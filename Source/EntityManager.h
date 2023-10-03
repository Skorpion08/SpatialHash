#pragma once

#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <iostream>

#include "TextureManager.h"
#include "Vector.h"
#include "Shape.h"
#include "sorted_vector.h"
#include "Column.h"

using EntityID = size_t;
using ComponentID = EntityID;
using ArchetypeID = EntityID;

using Type = sorted_vector;

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
	std::vector<Column> columns;
	std::vector<EntityID> id_table;
	size_t entityCount = 0;
};

struct EntityRecord
{
	Archetype* archetype;
	size_t row;
};

inline std::unordered_map<EntityID, EntityRecord> entityRecord;
inline std::unordered_map<Type, Archetype, TypeHash> typeToArchetype;
inline std::unordered_map<ComponentID, std::vector<Archetype*>> componentIndex;


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
	inline ComponentID GetID()
	{
		static ComponentID componentID = nextComponentID++;
		return componentID;
	}

	template <typename T, typename... Args>
	void Add(EntityID entityID, Args&&... args);
	
	// Meant to be used for a single entity only
	template <typename T>
	T* Get(EntityID entityID);

	void* GetC(EntityID entityID, ComponentID typeHandle) 
	{
		EntityRecord& record = entityRecord[entityID];
		int index = record.archetype->type.FindIndexFor(typeHandle);
		if (index == -1)
			return nullptr;

		return record.archetype->columns[index].GetAddress(record.row);
	}

	Sprite* AddSprite(EntityID entityID, SDL_Rect* src, SDL_Rect* dst, Texture* texture);
	Transform* AddTransform(EntityID entityID, Vector2 position = { 0,0 }, float rotation = 0.0f, Vector2 scale = { 0,0 });
	Kinematics* AddKinematics(EntityID entityID, Vector2 vel = { 0,0 }, Vector2 acc = { 0,0 }, float angularVel = 0.0f, float angularAcc = 0.0f);
	Collision* AddCollision(EntityID entityID, Shape2D* collider, bool blockCollision = true);
	Rigidbody* AddRigidbody(EntityID entityID, float mass, bool enableGravity, float gravityAcc = 9.81, float elasticity = 0.6, float staticFriction = 0.0, float dynamicFriction = 0.0);

	template <typename... Types>
	Archetype* QueryExact();

	template <typename... Types>
	std::vector<Archetype*> Query();

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

template <typename T, typename... Args>
void ECS::Add(EntityID entityID, Args&&... args)
{
	if (!entityRecord.contains(entityID))
		return;

	ComponentID componentID = GetID<T>();

	EntityRecord& record = entityRecord[entityID];
	Archetype* oldArchetype = record.archetype;
	Type& oldType = oldArchetype->type;
	Type newType = oldArchetype->type;
	auto& oldTable = oldArchetype->columns;
	newType.Insert(componentID);

	bool createNewArchetype = !typeToArchetype.contains(newType);

	Archetype* newArchetype = &typeToArchetype[newType];
	record.archetype = newArchetype;
	auto& columns = newArchetype->columns;
	size_t oldRow = record.row;
	int newCompIndex = newType.FindIndexFor(componentID);


	// j is the currently used table index from the previous archetype
	int j = 0;
	// Create a new archetype if we couldn't find one
	if (createNewArchetype)
	{
		columns.reserve(newType.Size());
		newArchetype->type = newType;
		for (int i = 0; i < newType.Size(); ++i)
		{
			// Add the newly created archetypes to matching id's
			componentIndex[newType[i]].emplace_back(newArchetype);

			if (i == newCompIndex)
			{
				columns.emplace_back(Column(typeid(T), sizeof(T)));
				continue;
			}
			columns.emplace_back(Column(oldTable[j].type, oldTable[j].element_size));
			++j;
		}
	}
	record.row = columns[0].m_count;

	j = 0;
	// Move over the data
	for (int i = 0; i < newType.Size(); ++i)
	{
		if (i == newCompIndex)
		{
			// we push in the new data and add the index
			columns[i].Insert<T>(std::move(args)...);
			
			newArchetype->id_table.resize(newArchetype->entityCount + 1);
			newArchetype->id_table[newArchetype->entityCount++] = entityID;
			continue;
		}

		Column* oldCol = &oldTable[j];

		// Move data to the new archetype
		columns[i].ResizeFor(1);
		memcpy(columns[i].GetAddress(columns[i].m_count++), oldCol->GetAddress(oldRow), columns[i].element_size);

		// Swap the data from the end to the index of moved entity (only if the entity isn't at the last index)
		EntityID idToSwap = oldArchetype->id_table[oldCol->m_count - 1];
		if (idToSwap != entityID)
		{
			entityRecord[idToSwap].row = oldRow;
			oldArchetype->id_table[oldRow] = idToSwap;
			memcpy(oldCol->GetAddress(oldRow), oldCol->GetAddress(--oldCol->m_count), sizeof(columns[j].element_size));
		}
		else
		{
			--oldCol->m_count;
		}
		++j;
	}
	--oldArchetype->entityCount;
}

template<typename T>
T* ECS::Get(EntityID entityID)
{
	EntityRecord& record = entityRecord[entityID];
	int index = record.archetype->type.FindIndexFor(GetID<T>());
	if (index == -1)
		return nullptr;

	return record.archetype->columns[index].Get<T>(record.row);
}

template<typename... Types>
inline Archetype* ECS::QueryExact()
{
	Type queriedType = { GetID<Types>()... };
	
	if (typeToArchetype.contains(queriedType))
	{
		return &typeToArchetype[queriedType];
	}
	return nullptr;
}

template<typename ...Types>
inline std::vector<Archetype*> ECS::Query()
{
	Type queriedType = { GetID<Types>()... };
	int n = queriedType.Size();

	// We need to find the vector of archetypes with the least amount
	int min = std::numeric_limits<int>::max();

	// Searched id with the lowest size
	int id = -1;
	for (int i = 0; i < n; ++i)
	{
		if (!componentIndex.contains(queriedType[i]))
			return {};

		if (componentIndex[queriedType[i]].size() < min)
			min = componentIndex[queriedType[i]].size(); id = queriedType[i];
	}

	std::vector<Archetype*>& archetypes = componentIndex[id];
	std::vector<Archetype*> result;

	// Pointer in the queried type
	int k = 1;

	for (int i = 0; i < archetypes.size(); ++i)
	{
		Type& currentType = archetypes[i]->type;
		Archetype* arch = archetypes[i];
		int l = arch->type.FindIndexFor(queriedType[0]);
		int r = arch->type.FindIndexFor(queriedType[n - 1]);

		if (l == -1 || r == -1)
			continue;

		// We can skip under these conditions as we do not need to waste time on slower checks
		if ((l == r && n == 1) || n == 2)
		{
			result.emplace_back(arch);
			continue;
		}

		// It's the size of the interval we need to check
		int s = r - l + 1;

		int misses = s-n, hits = 2;

		if (misses < 0)
			continue;
		
		k = 1; // reset the pointer

		for (int j = l+1; j < r; ++j)
		{
			// We can break if we used our misses
			if (misses < 0) break;

			if (currentType[j] == queriedType[k])
			{
				++hits;
				++k;
				// Add it to result if it passed
				if (hits == n) result.emplace_back(arch);
			}
			else
				--misses;
		}
	}
	return result;
}

// Creates an id for a type handle
#define getID(type) type##_ID

#define COMPONENT(type) ComponentID getID(type) = ecs.GetID<type>();

#define get(e, type) static_cast<type*>(ecs.GetC(e, getID(type)));
