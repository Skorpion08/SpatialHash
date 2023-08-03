#include "EntityManager.h"

EntityID EntityManager::CreateEnitity()
{
	entities.emplace_back(true);
	return nextEntityID++;
}

Sprite* EntityManager::AddSprite(EntityID entityID, SDL_Rect* src, SDL_Rect* dst, Texture* texture)
{
	if (EntityIDValid(entityID))
	{
		GetInstance().GetRegistry().sprites.emplace(entityID, Sprite(src, dst, texture));
		return &GetInstance().GetRegistry().sprites[entityID];
	}
	return nullptr;
}

Transform* EntityManager::AddTransform(EntityID entityID, Vector2 position, float rotation, Vector2 scale)
{
	if (EntityIDValid(entityID))
	{
		GetInstance().GetRegistry().transforms.emplace(entityID, Transform(position, rotation, scale));
		return &GetInstance().GetRegistry().transforms[entityID];
	}
	return nullptr;
}

Kinematics* EntityManager::AddKinematics(EntityID entityID, Vector2 vel, Vector2 acc, float angularVel, float angularAcc)
{
	if (EntityIDValid(entityID))
	{
		GetInstance().GetRegistry().kinematics.emplace(entityID, Kinematics(vel, acc, angularVel, angularAcc));
		return &GetInstance().GetRegistry().kinematics[entityID];
	}
	return nullptr;
}

void EntityManager::DestroyEntity(EntityID entityID)
{
	entities[entityID] = false;

	// deactivate other components
	registry.sprites.erase(entityID);
	registry.transforms.erase(entityID);
	registry.kinematics.erase(entityID);
	registry.collisionComponents.erase(entityID);
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
			sprite.dst->x = transform.pos.x - sprite.dst->w / 2.f;
			sprite.dst->y = transform.pos.y - sprite.dst->h / 2.f;
		}
	}
}

void SpriteSystem::Render()
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();
	
	for (auto& [entityID, sprite] : registry.sprites)
	{
		if (registry.transforms.contains(entityID))
		{
			auto& transform = registry.transforms[entityID];

			// We need to convert radians to degrees
			sprite.texture->Render(sprite.src, sprite.dst, transform.rotation* 57.295779513082320876798154814105, nullptr);
		}
		else
		{
			sprite.texture->Render(sprite.src, sprite.dst);
		}
	}
}

void TransformSystem::Update(double deltaTime)
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();
	
	for (auto& [entityID, kinematics] : registry.kinematics)
	{
		// Apply kinematics and motion
		auto& transform = registry.transforms[entityID];
		// Apply velocity
		transform.pos.x += kinematics.vel.x * deltaTime;
		transform.pos.y += kinematics.vel.y * deltaTime;

		// Move the collision component
		if (registry.collisionComponents.contains(entityID))
			registry.collisionComponents[entityID].collider->AddPosition(Vector2(kinematics.vel.x * deltaTime, kinematics.vel.y*deltaTime));

		// Apply acceleration
		kinematics.vel.x += kinematics.acc.x * deltaTime;
		kinematics.vel.y += kinematics.acc.y * deltaTime;

		// Apply angular velocity and acceleration
		transform.rotation += kinematics.angularVel * deltaTime;

		// Move the collision component
		if (registry.collisionComponents.contains(entityID))
			registry.collisionComponents[entityID].collider->AddRotation(kinematics.angularVel * deltaTime);

		kinematics.angularVel += kinematics.angularAcc * deltaTime;
	}
}

void TransformSystem::SetScale(EntityID entityID, Vector2 scale)
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();

	if (registry.transforms.contains(entityID))
	{
		// Calculate the delta and the add one
		Vector2 newScale = scale - registry.transforms[entityID].scale + Vector2(1, 1);

	}
}

void CollisionSystem::Update()
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();

	/*
	for (auto& [entityID, aabb] : registry.aabbs)
	{
		// Center the aabb
		auto& transform = registry.transforms[entityID];
		aabb.boundingBox->x = transform.posX - aabb.boundingBox->w / 2;
		aabb.boundingBox->y = transform.posY - aabb.boundingBox->h / 2;
	}
	*/
	for (auto& [entityID, CollisionComponent] : registry.collisionComponents)
	{
		CollisionComponent.collider->UpdateSideVectors();
	}
}

CollisionInfo CollisionSystem::AABBtoAABB(AABB& a, AABB& b)
{
	CollisionInfo info;

	// Setup the corners
	a.x = a.vertices[0].x;
	a.y = a.vertices[0].y;

	b.x = b.vertices[0].x;
	b.y = b.vertices[0].y;

	// Check if there is no overlap along any axis
	if (a.y + a.h <= b.y || a.y >= b.y + b.h || a.x + a.w <= b.x || a.x >= b.x + b.w)
	{
		info.overlap = false;
		info.mtv = { 0,0 };
		return info;
	}
	else // Colliding
	{
		// Calculate overlap
		int overlapX = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
		int overlapY = std::min(a.y + a.h, b.y + b.h) - std::max(a.y, b.y);

		// If the overlap result is positive it means there is overlap

		// Determine the minimum translation vector based on wich side is penetrated the least
		// The axis with less overlap is the penetrating one
		if (overlapX < overlapY) // On the x axis
		{
			if (a.x < b.x) // If penetrating from the left side
			{
				info.mtv.x = -overlapX;
			}
			else // Penetrating from the right side
			{
				info.mtv.x = overlapX;
			}
			info.mtv.y = 0; // No need to move it on the y axis
		}
		else // On the y axis
		{
			info.mtv.x = 0;
			if (a.y < b.y) // If penetrating from above
			{
				info.mtv.y = -overlapY;
			}
			else // Penetrating from the right side
			{
				info.mtv.y = overlapY;
			}
		}
		info.overlap = true;
		return info;
	}
}

CollisionInfo CollisionSystem::POLYtoPOLY(Shape2D& shapeA, Shape2D& shapeB)
{
	CollisionInfo info;

	auto& sidesA = shapeA.sides;
	auto& sidesB = shapeB.sides;

	// We use the normals as our axes for checking
	std::vector<Vector2> axes;
	axes.reserve(sidesA.size() + sidesB.size());
	
	for (size_t i = 0; i < sidesA.size(); ++i)
	{
		axes.emplace_back(Vector::GetNormal(Vector::GetNormalized(sidesA[i])));
	}
	for (size_t i = 0; i < sidesB.size(); ++i)
	{
		axes.emplace_back(Vector::GetNormal(Vector::GetNormalized(sidesB[i])));
	}

	float minOverlap = std::numeric_limits<float>::infinity();
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
		// We use negative numbers for no overlap
		float overlap = std::min(maxA, maxB) - std::max(minA, minB);
		//printf("%f\n", overlap);
		// If overlap length is negative that means there is no overlap
		if (overlap < 0)
		{
			// There is a gap and we can quit
			info.mtv = { 0,0 };
			info.overlap = false;
			//printf("no collision\n");
			return info;
		}
		else if (overlap < minOverlap)
		{
			minOverlap = overlap;

			// use operator overloading to improve this code

			info.mtv.x = axes[i].x * minOverlap;
			info.mtv.y = axes[i].y * minOverlap;
		}
	
	}
	// If we didn't find any seperating axis we are colliding
	//printf("collision\n");

	// We need to reverse the mtv direction if it doesn't point in the same direction as from B to A
	if (Vector::DotProduct(shapeA.center - shapeB.center, info.mtv) < 0)
	{
		info.mtv = -info.mtv;
	}

	info.overlap = true;

	return info;
}

void CollisionSystem::SolveCollisions()
{
	auto& manager = EntityManager::GetInstance();
	auto& entities = manager.GetEntities();
	auto& registry = manager.GetRegistry();
	
	//CollisionInfo info = AABBtoAABB(static_cast<AABB&>(*registry.collisionComponents[2].collider), static_cast<AABB&>(*registry.collisionComponents[3].collider));
	//registry.transforms[2].pos.x += info.mtv.x;
	//registry.transforms[2].pos.y += info.mtv.y;
	//registry.collisionComponents[2].collider->AddPosition(Vector2(info.mtv.x, info.mtv.y));
	
	/*
	auto& collisionComponent1 = registry.collisionComponents[0], collisionComponent2 = registry.collisionComponents[1];
	EntityID entity1ID = 0, entity2ID = 1;

	CollisionMethod method1 = collisionComponent1.collider->GetCollisionMethod();
	CollisionMethod method2 = collisionComponent2.collider->GetCollisionMethod();

	CollisionInfo info;

	if (method1 == CollisionMethod::AABB && method2 == CollisionMethod::AABB)
	{
		info = AABBtoAABB(static_cast<AABB&>(*collisionComponent1.collider), static_cast<AABB&>(*collisionComponent2.collider));
	}
	else
	{
		info = POLYtoPOLY(*collisionComponent1.collider, *collisionComponent2.collider);
	}

	registry.transforms[entity1ID].pos.x += info.mtv.x / 2;
	registry.transforms[entity1ID].pos.y += info.mtv.y / 2;
	registry.collisionComponents[entity1ID].collider->AddPosition(Vector2(info.mtv.x / 2, info.mtv.y / 2));

	registry.transforms[entity2ID].pos.x -= info.mtv.x / 2;
	registry.transforms[entity2ID].pos.y -= info.mtv.y / 2;
	registry.collisionComponents[entity2ID].collider->AddPosition(Vector2(-(info.mtv.x / 2), -(info.mtv.y / 2)));
	*/

	// Less but still naive approach
	for (auto& [entity1ID, collisionComponent1] : registry.collisionComponents)
	{
		for (auto& [entity2ID, collisionComponent2] : registry.collisionComponents)
		{
			// We can skip if it's the same entity
			if (entity1ID == entity2ID)
				continue;

			CollisionMethod method1 = collisionComponent1.collider->GetCollisionMethod();
			CollisionMethod method2 = collisionComponent2.collider->GetCollisionMethod();

			CollisionInfo info;

			if (method1 == CollisionMethod::AABB && method2 == CollisionMethod::AABB)
			{
				info = AABBtoAABB(static_cast<AABB&>(*collisionComponent1.collider), static_cast<AABB&>(*collisionComponent2.collider));
			}
			else
			{
				info = POLYtoPOLY(*collisionComponent1.collider, *collisionComponent2.collider);
			}
			registry.transforms[entity1ID].pos.x += info.mtv.x/2;
			registry.transforms[entity1ID].pos.y += info.mtv.y/2;
			registry.collisionComponents[entity1ID].collider->AddPosition(Vector2(info.mtv.x/2, info.mtv.y/2));

			registry.transforms[entity2ID].pos.x -= info.mtv.x / 2;
			registry.transforms[entity2ID].pos.y -= info.mtv.y / 2;
			registry.collisionComponents[entity2ID].collider->AddPosition(Vector2(-(info.mtv.x / 2), -(info.mtv.y / 2)));

		}
	}
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
