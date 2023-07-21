#include "EntityManager.h"

EntityID EntityManager::CreateEnitity()
{
	entities.push_back(true);
	return nextEntityID++;
}

void EntityManager::DestroyEntity(EntityID entityID)
{
	entities[entityID] = false;

	// deactivate other components
	registry.sprites.erase(entityID);
	registry.transforms.erase(entityID);
	registry.aabbs.erase(entityID);
	registry.sats.erase(entityID);

}


// Test a way to optimize the looping through by instead iterating through the map

void SpriteSystem::Update()
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();
	
	for (auto& [entityID, sprite] : registry.sprites)
	{
		if (registry.transforms.contains(entityID))
		{
			auto& transform = registry.transforms[entityID];

			// Center the texture
			sprite.dst->x = transform.posX - sprite.dst->w / 2;
			sprite.dst->y = transform.posY - sprite.dst->h / 2;
		}
	}
}

void SpriteSystem::Render()
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();
	
	for (auto& [entityID, sprite] : registry.sprites)
	{
		sprite.texture->Render(sprite.src, sprite.dst);
	}

}

void TransformSystem::Update(double deltaTime)
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();
	
	for (auto& [entityID, transform] : registry.transforms)
	{
		transform.posX += transform.velX * deltaTime;
		transform.posY += transform.velY * deltaTime;
		transform.velX += transform.accX * deltaTime;
		transform.velY += transform.accY * deltaTime;
	}
}

void CollisionSystem::Update()
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();

	for (auto& [entityID, aabb] : registry.aabbs)
	{
		// Center the aabb
		auto& transform = registry.transforms[entityID];
		aabb.boundingBox->x = transform.posX - aabb.boundingBox->w / 2;
		aabb.boundingBox->y = transform.posY - aabb.boundingBox->h / 2;
	}
}

CollisionInfo& CollisionSystem::IsCollidingAABB(SDL_Rect& a, SDL_Rect& b)
{
	CollisionInfo collisionInfo;

	// Check if there is no overlap along any axis
	if (a.y + a.h <= b.y || a.y >= b.y + b.h || a.x + a.w <= b.x || a.x >= b.x + b.w)
	{
		collisionInfo.overlap = false;
		collisionInfo.mtv = { 0,0 };
		return collisionInfo;
	}
	else // Colliding
	{
		// Calculate overlap
		int overlapX = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
		int overlapY = std::min(a.y + a.h, b.y + b.h) - std::max(a.y, b.y);

		// If the overlap result is positive it means there is overlap

		// Determine the minimum translation vector based on wich side is penetrated
		// The axis with less overlap is the penetrating one
		if (overlapX < overlapY) // On the x axis
		{
			if (a.x < b.x) // If penetrating from the left side
			{
				collisionInfo.mtv.x = -overlapX;
			}
			else // Penetrating from the right side
			{
				collisionInfo.mtv.x = overlapX;
			}
			collisionInfo.mtv.y = 0; // No need to move it on the y axis
		}
		else // On the y axis
		{
			collisionInfo.mtv.x = 0;
			if (a.y < b.y) // If penetrating from above
			{
				collisionInfo.mtv.y = -overlapY;
			}
			else // Penetrating from the right side
			{
				collisionInfo.mtv.y = overlapY;
			}
		}
		collisionInfo.overlap = true;
		return collisionInfo;
	}
}

CollisionInfo& CollisionSystem::IsCollidingSAT(Shape2D& shapeA, Shape2D& shapeB)
{
	CollisionInfo info;

	std::vector<Vector2> sidesA;
	std::vector<Vector2> sidesB;

	//sidesA.reserve(shapeA.vertices.size());
	//sidesB.reserve(shapeB.vertices.size());

	shapeA.GetSideVectors(sidesA);
	shapeB.GetSideVectors(sidesB);

	// TODO Check wether emplace back is a lot faster

	// We use the normals as our axes for checking
	std::vector<Vector2> axes;
	axes.resize(sidesA.size() + sidesB.size());
	
	for (size_t i = 0; i < sidesA.size(); ++i)
	{
		axes.emplace_back(Vector::GetNormal(Vector::GetNormalized(sidesA[i])));
	}
	for (size_t i = 0; i < sidesB.size(); ++i)
	{
		axes.emplace_back(Vector::GetNormal(Vector::GetNormalized(sidesB[i])));
	}
	// Loop over all the axes
	for (uint32_t i = 0; i < axes.size(); ++i)
	{
		// Project all of the vertices of A and B onto the axis

		// Min and max vertices of each shape on the axis
		float minA = Vector::DotProduct(shapeA.vertices[0], axes[i]), maxA = minA;
		float minB = Vector::DotProduct(shapeB.vertices[0], axes[i]), maxB = minB;
	
		for (uint32_t j = 0; j < shapeA.vertices.size(); ++j) // Find the min and max for shape A on this axis
		{
			// Project the vertex onto the axis
			float projection = Vector::DotProduct(shapeA.vertices[j], axes[i]);

			minA = std::min(minA, projection);
			maxA = std::max(maxA, projection);
		}

		for (uint32_t j = 0; j < shapeB.vertices.size(); ++j) // The same for B
		{
			// Project the vertex onto the axis
			float projection = Vector::DotProduct(shapeB.vertices[j], axes[i]);

			minB = std::min(minB, projection);
			maxB = std::max(maxB, projection);
		}
		if (minA - maxB > 0 || minB - maxA > 0)
		{
			// There is a gap and we can quit
			info.overlap = false;
			//printf("no collision\n");
			return info;
		}
	}
	// If we didn't find any seperating axis we are colliding
	//printf("collision\n");

	// Not finished code
	info.overlap = true;

	return info;
}

void CollisionSystem::SolveCollisions()
{
	auto& manager = EntityManager::GetInstance();
	auto& entities = manager.GetEntities();
	auto& registry = manager.GetRegistry();
	
	/*
	CollisionInfo info = IsCollidingAABB(*registry.aabbs[0].boundingBox, *registry.aabbs[manager.NumberOfEntities()-1].boundingBox);
	registry.transforms[0].posX += info.mtv.x;
	registry.transforms[0].posY += info.mtv.y;
	*/
	//printf("x: %f y: %f\n", info.mtv.x, info.mtv.y);

	CollisionInfo info = IsCollidingSAT(*registry.sats[0], *registry.sats[1]);

	// Naive approach
	/*
	for (EntityID e = 0; e < EntityManager::NumberOfEntities(); e++)
	{
		for (EntityID e2 = 0; e2 < EntityManager::NumberOfEntities(); e2++)
		{
			// If enity is destroyed we can skip (entities are bools)
			if (!entities[e] || !entities[e2] || e == e2)
				continue;


			if (registry.aabbs.contains(e) && registry.aabbs.contains(e2))
			{
				auto& aabb = registry.aabbs[e];
				auto& aabb2 = registry.aabbs[e2];

			}
		}
	}
	*/
}
