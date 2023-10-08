#pragma once

#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <iostream>

#include "sorted_vector.h"
#include "Column.h"

using ID = uint64_t;
using EntityID = ID;
using ArchetypeID = ID;

using Type = sorted_vector;

struct Archetype
{
	ArchetypeID id;
	Type type;

	std::vector<EntityID> id_table;
	// TO DO: Store columns in a pointer so archetypes with tag point to the same column
	std::vector<Column> columns;
	size_t entityCount = 0;

	// Number of data entities attached to it
	uint16_t dataCount = 0;
};

struct EntityRecord
{
	Archetype* archetype;
	size_t row;
};

inline std::unordered_map<EntityID, EntityRecord> entityRecord;
inline std::unordered_map<Type, Archetype, TypeHash> typeToArchetype;
inline std::unordered_map<ID, std::vector<Archetype*>> componentIndex;

struct World
{
	//std::vector<>
};

inline EntityID nextEntityID = 0;
namespace ECS
{

	// Returns its ID
	EntityID NewEnitity();

	//template <typename T>
	//inline ComponentID GetID()
	//{
	//	static ComponentID componentID = nextComponentID++;
	//	return componentID;
	//}

	inline ID NewID()
	{
		return nextEntityID++;
	}

	// It adds an id and doesnt call the constructor
	void Add(EntityID entityID, ID newID);

	template <typename T, typename... Args>
	void Add(EntityID entityID, ID componentID, Args&&... args);
	
	// Meant to be used for a single entity only
	template <typename T>
	inline T* Get(EntityID entityID);

	void* Get(EntityID entityID, ID id);

	bool Has(EntityID entity, ID id);

	template <typename... Types>
	Archetype* QueryExact();

	std::vector<Archetype*> Query(Type&& queriedType);

	template <typename... Types>
	std::vector<Archetype*> Query();

	// Returns the id for a new component
	ID InitComponent(const std::type_info& ti, uint32_t size);
};


#define getID(type) type##_ID

struct Data
{
	const std::type_info& ti;
	uint32_t size;
};

inline const ID getID(Data) = ECS::NewID();

template <typename T, typename... Args>
void ECS::Add(EntityID entityID, ID componentID,  Args&&... args)
{
	if (!entityRecord.contains(entityID))
		return;

	//ID componentID = getID(T);

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

			if (!Has(newType[i], getID(Data))) continue;

			if (i == newCompIndex)
			{
				columns.emplace_back(Column(typeid(T), sizeof(T)));
				continue;
			}
			columns.emplace_back(Column(oldTable[j].type, oldTable[j].element_size));
			++j;
		}
	}
	record.row = newArchetype->entityCount;

	j = 0;
	// Move over the data
	for (int i = 0; i < newType.Size(); ++i)
	{
		if (!Has(newType[i], getID(Data))) continue;

		if (i == newCompIndex)
		{
			// we push in the new data and add the index
			columns[i].PushBack<T>(std::move(args)...);
			
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
inline T* ECS::Get(EntityID entityID)
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

#define COMPONENT(type) ID getID(type) = ECS::InitComponent(typeid(type), sizeof(type));

#define TAG(name) ID name = ECS::NewEnitity();

#define get(e, type) static_cast<type*>(ECS::Get(e, getID(type)))


#define AddTag(e, name) ECS::Add(e, name)
// Doesn't invoke the constructor
#define AddID(e, id) ECS::Add(e, id)
// Invokes the constructor (uses templates)
#define AddData(e, type, ...) ECS::Add<type>(e, getID(type), __VA_ARGS__)