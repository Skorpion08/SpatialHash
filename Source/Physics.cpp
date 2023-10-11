#include "Physics.h"

void Physics::UpdateTransform(double deltaTime)
{
	std::vector<Archetype*> query = ECS::Query({ getID(Transform), getID(Kinematics) });

	for (int k = 0; k < query.size(); ++k)
	{
		Archetype* arch = query[k];
		Transform* transforms = static_cast<Column<Transform>*>(arch->columns[arch->type.FindIndexFor(getID(Transform))])->Get(0);
		Kinematics* kinematics = static_cast<Column<Kinematics>*>(arch->columns[arch->type.FindIndexFor(getID(Kinematics))])->Get(0);
		for (int i = 0; i < arch->entityCount; ++i)
		{
			kinematics[i].vel += kinematics[i].acc * deltaTime;
			transforms[i].pos += kinematics[i].vel * deltaTime;
		}
	}
}

void Physics::UpdateColliders()
{
	std::vector<Archetype*> query = ECS::Query({ getID(Transform), getID(Collision) });

	for (int k = 0; k < query.size(); ++k)
	{
		Archetype* arch = query[k];
		Transform* transforms = static_cast<Column<Transform>*>(arch->columns[arch->type.FindIndexFor(getID(Transform))])->Get(0);
		Collision* collisionComps = static_cast<Column<Collision>*>(arch->columns[arch->type.FindIndexFor(getID(Collision))])->Get(0);
		for (int i = 0; i < arch->entityCount; ++i)
		{
			collisionComps[i].collider->MoveBy(transforms[i].pos - collisionComps[i].collider->center);

			// update the side vectors to make collision work
			collisionComps[i].collider->UpdateSideVectors();
		}
	}
}

CollisionInfo Physics::AABBtoAABB(AABB& a, AABB& b)
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

CollisionInfo Physics::POLYtoPOLY(Shape2D& shapeA, Shape2D& shapeB)
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

void Physics::FindSolveCollisions()
{
	std::vector<Archetype*> query = ECS::Query({ getID(Collision) });

	for (int k = 0; k < query.size(); ++k)
	{
		Archetype* arch = query[k];
		Collision* collisionComps = static_cast<Column<Collision>*>(arch->columns[arch->type.FindIndexFor(getID(Collision))])->Get(0);
		for(int i = 0; i < arch->entityCount; ++i)
		{
			collisionComps
		}
	}

	// Less but still naive approach
	for (auto& [entity1ID, collisionComponent1] : registry.collisionComponents)
	{
		for (auto& [entity2ID, collisionComponent2] : registry.collisionComponents)
		{
			// We can skip if it's the same entity
			if (entity1ID == entity2ID || (entities.at(entity1ID).movability == Static && entities.at(entity2ID).movability == Static))
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

			if (collisionComponent1.blockCollision && collisionComponent2.blockCollision && info.overlap) // We can solve the collision
			{
				Vector2 displacement1;
				Vector2 displacement2;

				registry.transforms[entity1ID].pos += info.mtv / 2;
				displacement1 += info.mtv / 2;
				collisionComponent1.collider->AddPosition(info.mtv / 2);

				registry.transforms[entity2ID].pos -= info.mtv / 2;
				displacement2 -= info.mtv / 2;
				collisionComponent2.collider->AddPosition(-info.mtv / 2);

				if (manager.GetEntities()[entity1ID].movability == Static)
				{
					registry.transforms[entity1ID].pos -= info.mtv / 2;
					displacement1 -= info.mtv / 2;
					collisionComponent1.collider->AddPosition(-info.mtv);

					registry.transforms[entity2ID].pos -= info.mtv / 2;
					displacement2 -= info.mtv / 2;
					collisionComponent2.collider->AddPosition(-info.mtv);
				}
				if (manager.GetEntities()[entity2ID].movability == Static)
				{
					registry.transforms[entity2ID].pos += info.mtv / 2;
					displacement2 += info.mtv / 2;
					collisionComponent2.collider->AddPosition(info.mtv / 2);

					registry.transforms[entity1ID].pos += info.mtv / 2;
					displacement1 += info.mtv / 2;
					collisionComponent1.collider->AddPosition(info.mtv);
				}

				if (registry.rigidbodies.contains(entity1ID) && registry.rigidbodies.contains(entity2ID))
				{
					// If there's an overlap but no mtv it means shapes barely stick to each other
					if (info.mtv.x == 0 && info.mtv.y == 0)
						continue;

					Rigidbody& rb1 = registry.rigidbodies[entity1ID];
					Rigidbody& rb2 = registry.rigidbodies[entity2ID];

					// We need to calculate the velocity at the time of collision and not at the time of overlaps

					Vector2 relativeVelVector = registry.kinematics[entity2ID].vel - registry.kinematics[entity1ID].vel;
					Vector2 normal = Vector::GetNormalized(info.mtv);
					//Vector2 normal = Vector::GetNormalized(info.mtv);

					float relativeVel = Vector::DotProduct(relativeVelVector, normal);

					if (relativeVel > 0)
					{
						//printf("relative vel: %f\n", relativeVel);
						float invMass1, invMass2;
						if (manager.GetEntities()[entity1ID].movability == Dynamic)
							invMass1 = 1.0 / rb1.mass;
						else
							invMass1 = 0;

						if (manager.GetEntities()[entity2ID].movability == Dynamic)
							invMass2 = 1.0 / rb2.mass;
						else
							invMass2 = 0;

						// Calculate impulse
						float impulseScalar = -(1 + (rb1.elasticity + rb2.elasticity) / 2) *
							relativeVel /
							(invMass1 + invMass2);
						Vector2 impulse = normal * impulseScalar;

						// Apply impulse to kinematics
						registry.kinematics[entity1ID].vel -= impulse * invMass1;
						registry.kinematics[entity2ID].vel += impulse * invMass2;

						relativeVelVector = registry.kinematics[entity2ID].vel - registry.kinematics[entity1ID].vel;

						relativeVel = Vector::DotProduct(relativeVelVector, normal);

						// Apply friction
						float sf = (rb1.staticFriction + rb2.staticFriction) / 2;
						float df = (rb1.dynamicFriction + rb2.dynamicFriction) / 2;

						Vector2 tangent = Vector::GetNormalized(relativeVelVector - (normal * relativeVel));
						float frictionScalar = -Vector::DotProduct(relativeVelVector, tangent) / (invMass1 + invMass2);

						Vector2 frictionImpulse = frictionScalar * tangent;



						if (abs(frictionScalar) < abs(impulseScalar) * sf)
						{
							frictionImpulse = frictionScalar * tangent;
						}
						else
						{
							frictionImpulse = impulseScalar * tangent * df;
						}

						// Apply friction impulse to kinematics
						registry.kinematics[entity1ID].vel -= frictionImpulse * invMass1;
						registry.kinematics[entity2ID].vel += frictionImpulse * invMass2;
					}
				}
			}
		}
	}
}
