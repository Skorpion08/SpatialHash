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

Collision* EntityManager::AddCollision(EntityID entityID, Shape2D* collider, bool blockCollision)
{
	if (EntityIDValid(entityID))
	{
		GetInstance().GetRegistry().collisionComponents.emplace(entityID, Collision(collider, blockCollision));
		return &GetInstance().GetRegistry().collisionComponents[entityID];
	}
	return nullptr;
}

Rigidbody* EntityManager::AddRigidbody(EntityID entityID, float mass, bool enableGravity, float gravityAcc)
{
	if (EntityIDValid(entityID))
	{
		GetInstance().GetRegistry().rigidbodies.emplace(entityID, Rigidbody(mass, enableGravity, gravityAcc));
		return &GetInstance().GetRegistry().rigidbodies[entityID];
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

void SpriteSystem::Update(Vector2 cameraPos)
{
	auto& manager = EntityManager::GetInstance();
	auto& registry = manager.GetRegistry();
	
	for (auto& [entityID, sprite] : registry.sprites)
	{
		if (registry.transforms.contains(entityID))
		{
			auto& transform = registry.transforms[entityID];
			// Center the texture
			sprite.dst->x = (transform.pos.x - sprite.dst->w / 2.f) - cameraPos.x;
			sprite.dst->y = (transform.pos.y - sprite.dst->h / 2.f) - cameraPos.y;
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

		if (registry.rigidbodies.contains(entityID) && registry.rigidbodies[entityID].enableGravity == true)
		{
			float gravityForce = registry.rigidbodies[entityID].mass * registry.rigidbodies[entityID].gravityAcc;
			kinematics.acc.y = gravityForce / registry.rigidbodies[entityID].mass;
		}

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
		if (registry.transforms.contains(entityID))
		{
			CollisionComponent.collider->AddPosition(registry.transforms[entityID].pos - CollisionComponent.collider->center);
		}
		
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
			// This code's a mess
			/*
			if (registry.kinematics.contains(entity1ID))
			{
				registry.transforms[entity1ID].pos.x += info.mtv.x / 2;
				registry.transforms[entity1ID].pos.y += info.mtv.y / 2;
				registry.collisionComponents[entity1ID].collider->AddPosition(Vector2(info.mtv.x / 2, info.mtv.y / 2));

				if (registry.kinematics.contains(entity2ID))
				{
					registry.transforms[entity2ID].pos.x -= info.mtv.x / 2;
					registry.transforms[entity2ID].pos.y -= info.mtv.y / 2;
					registry.collisionComponents[entity2ID].collider->AddPosition(Vector2(-(info.mtv.x / 2), -(info.mtv.y / 2)));
				}
				else
				{
					registry.transforms[entity1ID].pos.x += info.mtv.x / 2;
					registry.transforms[entity1ID].pos.y += info.mtv.y / 2;
					registry.collisionComponents[entity1ID].collider->AddPosition(Vector2(info.mtv.x / 2, info.mtv.y / 2));
				}
			}
			else if (registry.kinematics.contains(entity2ID))
			{
				registry.transforms[entity2ID].pos.x -= info.mtv.x / 2;
				registry.transforms[entity2ID].pos.y -= info.mtv.y / 2;
				registry.collisionComponents[entity2ID].collider->AddPosition(Vector2(-(info.mtv.x / 2), -(info.mtv.y / 2)));

				if (registry.kinematics.contains(entity1ID))
				{
					registry.transforms[entity1ID].pos.x += info.mtv.x / 2;
					registry.transforms[entity1ID].pos.y += info.mtv.y / 2;
					registry.collisionComponents[entity1ID].collider->AddPosition(Vector2(info.mtv.x / 2, info.mtv.y / 2));
				}
				else
				{
					registry.transforms[entity2ID].pos.x -= info.mtv.x / 2;
					registry.transforms[entity2ID].pos.y -= info.mtv.y / 2;
					registry.collisionComponents[entity2ID].collider->AddPosition(Vector2(-(info.mtv.x / 2), -(info.mtv.y / 2)));
				}
			}
			else
			{
				registry.transforms[entity1ID].pos.x += info.mtv.x / 2;
				registry.transforms[entity1ID].pos.y += info.mtv.y / 2;
				registry.collisionComponents[entity1ID].collider->AddPosition(Vector2(info.mtv.x / 2, info.mtv.y / 2));

				registry.transforms[entity2ID].pos.x -= info.mtv.x / 2;
				registry.transforms[entity2ID].pos.y -= info.mtv.y / 2;
				registry.collisionComponents[entity2ID].collider->AddPosition(Vector2(-(info.mtv.x / 2), -(info.mtv.y / 2)));
			}
			*/

			if (collisionComponent1.blockCollision && collisionComponent2.blockCollision && info.overlap) // We can solve the collision
			{
				if (registry.rigidbodies.contains(entity1ID) && registry.rigidbodies.contains(entity2ID))
				{

					Rigidbody& rb1 = registry.rigidbodies[entity1ID];
					Rigidbody& rb2 = registry.rigidbodies[entity2ID];

					Vector2 relativeVelVector = registry.kinematics[entity2ID].vel - registry.kinematics[entity1ID].vel;

					Vector2 normal = Vector::GetNormalized(info.mtv);

					float relativeVel = Vector::DotProduct(relativeVelVector, normal);
					
					// Calculate impulse
					float impulseScalar = -(1 + 1 /* + (rb1.elasticity + rb2.elasticity) / 2*/) *
						relativeVel /
						(1.0f / rb1.mass + 1.0f / rb2.mass);

					//printf("scalar: %f\n", impulseScalar);

					Vector2 impulse = normal * impulseScalar;
					// Apply impulse to kinematics
					registry.kinematics[entity1ID].vel -= impulse * (1.0f / rb1.mass);
					registry.kinematics[entity2ID].vel += impulse * (1.0f / rb2.mass);
				}
				registry.transforms[entity1ID].pos.x += info.mtv.x / 2;
				registry.transforms[entity1ID].pos.y += info.mtv.y / 2;
				collisionComponent1.collider->AddPosition(Vector2(info.mtv.x / 2, info.mtv.y / 2));

				registry.transforms[entity2ID].pos.x -= info.mtv.x / 2;
				registry.transforms[entity2ID].pos.y -= info.mtv.y / 2;
				collisionComponent2.collider->AddPosition(Vector2(-(info.mtv.x / 2), -(info.mtv.y / 2)));
			}
		}
	}
}
