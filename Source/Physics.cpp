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
		Archetype* arch1 = query[k];
		Collision* collisionComps1 = static_cast<Column<Collision>*>(arch1->columns[arch1->type.FindIndexFor(getID(Collision))])->Get(0);

		for (int i = 0; i < arch1->entityCount; ++i)
		{
			Collision& collisionComp1 = collisionComps1[i];
			for (int l = 0; l < query.size(); ++l)
			{
				Archetype* arch2 = query[l];
				Collision* collisionComps2 = static_cast<Column<Collision>*>(arch2->columns[arch2->type.FindIndexFor(getID(Collision))])->Get(0);
				for (int j = 0; j < arch2->entityCount; ++j)
				{
					if (arch1->id_table[i] == arch1->id_table[j])
						continue;

					Collision& collisionComp2 = collisionComps1[j];

					CollisionMethod method1 = collisionComp1.collider->GetCollisionMethod();
					CollisionMethod method2 = collisionComp2.collider->GetCollisionMethod();

					CollisionInfo info;

					if (method1 == CollisionMethod::AABB && method2 == CollisionMethod::AABB)
					{
						info = AABBtoAABB(static_cast<AABB&>(*collisionComp1.collider), static_cast<AABB&>(*collisionComp2.collider));
					}
					else
					{
						info = POLYtoPOLY(*collisionComp1.collider, *collisionComp2.collider);
					}

					//std::cout << "Colliding = " << (info.overlap ? "true\n" : "false\n");

				}
			}
		}
	}
}
