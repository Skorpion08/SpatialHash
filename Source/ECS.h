#pragma once

#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
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
	//std::vector<Column> columns;
	std::vector<ColumnBase*> columns;
	size_t entityCount = 0;

	// Number of data entities attached to it
	uint16_t dataCount = 0;

	bool Has(ID id) { return type.FindIndexFor(id) != -1; }
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

	void RemoveEntity(EntityID entityID);

	// Only for adding plain id's without any type
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

	// Returns the id for a new component
	ID InitComponent(const char* name, uint32_t size);
};


#define getID(type) type##_ID

struct Data
{
	std::string name;
	uint32_t size;
};

inline const ID getID(Data) = ECS::NewID();

template <typename T, typename... Args>
void ECS::Add(EntityID entityID, ID componentID,  Args&&... args)
{
	if (!entityRecord.contains(entityID))
		return;

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
				columns.emplace_back(new Column<T>);
				continue;
			}
			columns.emplace_back(oldTable[j]->CloneType());
			++j;
		}
	}
	record.row = newArchetype->entityCount;

	j = 0;
	// Move over the data
	for (int i = 0; i < newType.Size(); ++i)
	{

		if (i == newCompIndex)
		{
			// we push in the new data and add the index
			static_cast<Column<T>*>(columns[i])->PushBack(std::move(args)...);
			
			newArchetype->id_table.resize(newArchetype->entityCount + 1);
			newArchetype->id_table[newArchetype->entityCount] = entityID;
			continue;
		}
		if (!Has(newType[i], getID(Data))) continue;

		ColumnBase* oldCol = oldTable[j];

		// Move data to the new archetype
		columns[i]->MoveFrom(oldCol, oldRow);

		// Swap the data from the end to the index of moved entity (only if the entity isn't at the last index)
		EntityID idToSwap = oldArchetype->id_table[oldArchetype->entityCount-1];
		if (idToSwap != entityID)
		{
			entityRecord[idToSwap].row = oldRow;
			oldArchetype->id_table[oldRow] = idToSwap;
		}
		++j;
	}
	--oldArchetype->entityCount;
	++newArchetype->entityCount;
}
//
//template<typename T>
//inline T* ECS::Get(EntityID entityID)
//{
//	EntityRecord& record = entityRecord[entityID];
//	int index = record.archetype->type.FindIndexFor(GetID<T>());
//	if (index == -1)
//		return nullptr;
//
//	return record.archetype->columns[index].Get(record.row);
//}

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

// Creates an id for a type handle
#define COMPONENT(type) ID getID(type) = ECS::InitComponent(typeid(type).name(), sizeof(type));

#define TAG(name) ID name = ECS::NewEnitity();

#define GetData(e, type) static_cast<type*>(ECS::Get(e, getID(type)))

#define GetColumn(archetype, dataType) static_cast<Column<dataType>*>(archetype->columns[archetype->type.FindIndexFor(getID(dataType))])->Get(0)

#define AddTag(e, name) ECS::Add(e, name)

// Invokes the constructor (uses templates)
#define AddData(e, type, ...) ECS::Add<type>(e, getID(type), __VA_ARGS__)