#include "ECS.h"

EntityID ECS::NewEnitity()
{
	EntityID entityID = ECS::NewID();

	Archetype* defaultArchetype = &typeToArchetype[{}];

	size_t count = defaultArchetype->entityCount;
	auto& id_table = defaultArchetype->id_table;

	// we need to prevent the destructor from being called
	entityRecord.emplace(entityID, EntityRecord(defaultArchetype, count));

	if (id_table.capacity() < count + 1)
	{
		id_table.reserve(id_table.capacity() * 2);
	}

	id_table.resize(count + 1);
	id_table[defaultArchetype->entityCount++] = entityID;
	return entityID;
}

void ECS::RemoveEntity(EntityID entityID)
{
	if (!entityRecord.contains(entityID))
		return;

	EntityRecord& record = entityRecord[entityID];
	size_t row = record.row;
	for (int i = 0; i < record.archetype->dataCount; ++i)
	{
		record.archetype->columns[i]->Destroy(row);
	}
	// Swap the id
	record.archetype->id_table[row] = record.archetype->id_table[record.archetype->entityCount--];

	entityRecord.erase(entityID);
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
		// Reserve memory for an additional column only if we're creating additional data column
		newArchetype->dataCount = oldArchetype->dataCount + Has(newID, getID(Data)) ? 1 : 0;
		columns.reserve(newArchetype->dataCount);

		newArchetype->type = newType;
		for (int i = 0; i < newType.Size(); ++i)
		{
			// Add the newly created archetypes to matching id's
			componentIndex[newType[i]].emplace_back(newArchetype);

			if (!Has(newType[i], getID(Data))) continue;

			// We can ommit this as we don't create any columns
			//if (newIDIndex == i)
			//{
			//	Data* data = get(newID, Data);
			//	columns.emplace_back(data->ti, data->size);
			//	continue;
			//}

			columns.emplace_back(oldTable[j]->CloneType());
			++j;
		}
	}
	record.row = newArchetype->entityCount;
	
	j = 0;
	// Move over the data
	for (int i = 0; i < newType.Size(); ++i)
	{
		// Shit breaks when a tag is the one we're adding
		if (i == newIDIndex)
		{
			newArchetype->id_table.resize(newArchetype->entityCount + 1);
			newArchetype->id_table[newArchetype->entityCount] = entityID;
			continue;
		}
		if (!Has(newType[i], getID(Data))) continue;

		ColumnBase* oldCol = oldTable[j];

		// Move data to the new archetype
		columns[i]->MoveFrom(oldCol, oldRow);

		// Swap the data from the end to the index of moved entity (only if the entity isn't at the last index)
		EntityID idToSwap = oldArchetype->id_table[oldArchetype->entityCount - 1];
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

void* ECS::Get(EntityID entityID, ID id)
{
	EntityRecord& record = entityRecord[entityID];
	int index = record.archetype->type.FindIndexFor(id);
	if (index == -1)
		return nullptr;
	return record.archetype->columns[index]->GetAddress(record.row);
}

bool ECS::Has(EntityID entity, ID id)
{
	if (entityRecord.contains(entity) && entityRecord[entity].archetype->type.FindIndexFor(id) != -1)
		return true;
	else
		return false;
}

std::vector<Archetype*> ECS::Query(Type&& queriedType)
{
	int n = queriedType.Size();

	if (n == 0) return { &typeToArchetype[{}] };

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

		if (l == -1 || r == -1 || arch->entityCount <= 0)
			continue;

		// We can skip under these conditions as we do not need to waste time on slower checks
		if ((l == r && n == 1) || n == 2)
		{
			result.emplace_back(arch);
			continue;
		}

		// It's the size of the interval we need to check
		int s = r - l + 1;

		int misses = s - n, hits = 2;

		if (misses < 0)
			continue;

		k = 1; // reset the pointer

		for (int j = l + 1; j < r; ++j)
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

ID ECS::InitComponent(const char* name, uint32_t size)
{
	ID component = ECS::NewID();
	// Store Component as an entity
	entityRecord.emplace(component, EntityRecord(&typeToArchetype[{getID(Data)}], 0));

	EntityRecord& record = entityRecord[component];
	Archetype* arch = record.archetype;
	arch->type = { getID(Data) };

	// We can remove resize if we make this arch in a world setup
	arch->columns.emplace_back(new Column<Data>);
	static_cast<Column<Data>*>(arch->columns[0])->PushBack(name, size);

	// Setup the info for record
	arch->id_table.resize(record.archetype->entityCount + 1);
	arch->id_table[record.archetype->entityCount] = component;
	record.row = arch->entityCount++;
	return component;
}

Archetype::~Archetype()
{
	for (int i = 0; i < columns.size(); ++i)
		delete columns[i];
}

void* Archetype::GetColumnPtr(ID id)
{
	auto index = this->type.FindIndexFor(id);
	if (index != -1)
	{
		return this->columns[index]->GetAddress(0);
	}
	return nullptr;
}
