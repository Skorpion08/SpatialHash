#include "ECS.h"

EntityID ECS::CreateEnitity()
{
	EntityID entityID = ECS::NewID();
	entityRecord.emplace(entityID, EntityRecord(&typeToArchetype[{}], 0));
	return entityID;
}

void ECS::Add(EntityID entityID, ID newID)
{
	// We can quit if the entity doesnt exist
	if (!entityRecord.contains(entityID))
		return;

	EntityRecord& record = entityRecord[entityID];
	Archetype* oldArchetype = record.archetype;
	Type& oldType = oldArchetype->type;
	Type newType = oldArchetype->type;
	auto& oldTable = oldArchetype->columns;
	newType.Insert(newID);

	bool createNewArchetype = !typeToArchetype.contains(newType);

	Archetype* newArchetype = &typeToArchetype[newType];
	record.archetype = newArchetype;
	auto& columns = newArchetype->columns;
	size_t oldRow = record.row;
	int newIDIndex = newType.FindIndexFor(newID);

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

			if (newIDIndex == i) continue;

			columns.emplace_back(Column(oldTable[j].type, oldTable[j].element_size));
			++j;
		}
	}
	record.row = columns[0].m_count;


	j = 0;
	// Move over the data
	for (int i = 0; i < newType.Size(); ++i)
	{
		if (i == newID) continue;

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

void* ECS::Get(EntityID entityID, ID id)
{
	EntityRecord& record = entityRecord[entityID];
	int index = record.archetype->type.FindIndexFor(id);
	if (index == -1)
		return nullptr;

	return record.archetype->columns[index].GetAddress(record.row);
}

void ECS::InitComponent(ID component, const std::type_info& ti, uint32_t size)
{
	// Store Component as an entity
	entityRecord.emplace(component, EntityRecord(&typeToArchetype[{getID(Data)}], 0));

	EntityRecord record = entityRecord[component];
	Archetype* arch = record.archetype;
	arch->type = { getID(Data) };

	// We can remove resize if we make this arch in a world setup
	arch->columns.emplace_back(typeid(Data), size);
	Data* data = arch->columns[0].Insert<Data>(ti, size);

	// Setup the info for record
	arch->id_table.resize(record.archetype->entityCount + 1);
	arch->id_table[record.archetype->entityCount] = component;
	record.row = arch->entityCount++;
	static int i = 0;
	std::cout << arch->columns[0].Get<Data>(i++)->size<<'\n';
}
