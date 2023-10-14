#include "Physics.h"

void Physics::UpdateTransform(double deltaTime)
{
	std::vector<Archetype*> query = ECS::Query({ getID(Transform), getID(Kinematics) });

	for (int k = 0; k < query.size(); ++k)
	{
		Archetype* arch = query[k];
		Transform* transforms = GetColumn(arch, Transform);
		Kinematics* kinematics = GetColumn(arch, Kinematics);
		for (int i = 0; i < arch->entityCount; ++i)
		{
			Kinematics& kinematic = kinematics[i];

			// Calculate forces
			Vector2 force;

			Rigidbody* rb = GetColumn(arch, Rigidbody);
			force.x += (kinematic.vel.x * kinematic.vel.x * rb[i].linearDrag) / 100;
			force.y += (kinematic.vel.y * kinematic.vel.y * rb[i].linearDrag) / 100;


			// Check if the force should reverse direction based on velocity
			if (force.x > 0 && kinematic.vel.x > 0)
				force.x = -force.x;
			if (force.y > 0 && kinematic.vel.y > 0)
				force.y = -force.y;

			kinematic.vel += (kinematic.acc + force/ rb[i].mass) * deltaTime;
			transforms[i].pos += kinematic.vel * deltaTime;
		}
	}
}

void Physics::UpdateColliders()
{
	std::vector<Archetype*> query = ECS::Query({ getID(Transform), getID(Collision) });

	for (int k = 0; k < query.size(); ++k)
	{
		Archetype* arch = query[k];
		Transform* transforms = GetColumn(arch, Transform);
		Collision* collisionComps = GetColumn(arch, Collision);
		for (int i = 0; i < arch->entityCount; ++i)
		{
			Shape2D* collider = collisionComps[i].collider.get();

			collider->MoveBy(transforms[i].pos - collider->center);

			// update the side vectors to make collision work
			collider->UpdateSideVectors();
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
		Collision* collisionComps1 = GetColumn(arch1, Collision);

		Rigidbody* rigidbodies1 = GetColumn(arch1, Rigidbody);

		for (int i = 0; i < arch1->entityCount; ++i)
		{
			Collision& collisionComp1 = collisionComps1[i];
			for (int l = 0; l < query.size(); ++l)
			{
				Archetype* arch2 = query[l];
				Collision* collisionComps2 = GetColumn(arch2, Collision);

				Rigidbody* rigidbodies2 = GetColumn(arch2, Rigidbody);

				for (int j = 0; j < arch2->entityCount; ++j)
				{
					if (arch1->id_table[i] == arch2->id_table[j])
						continue;

					Collision& collisionComp2 = collisionComps2[j];

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

					if (info.overlap && collisionComp1.blockCollision && collisionComp2.blockCollision) // We can solve the collision
					{
						Vector2 displacement1;
						Vector2 displacement2;

						if (arch1->type.FindIndexFor(getID(Transform)) != -1 && arch1->type.FindIndexFor(getID(Transform)) != -1)
						{
							Transform* transforms1 = GetColumn(arch1, Transform);
							//Transform* transforms1 = static_cast<Column<Transform>*>(arch1->columns[arch1->type.FindIndexFor(getID(Transform))])->Get(0);
							Transform* transforms2 = GetColumn(arch2, Transform);;

							float massRatio1 = 0.5;
							float massRatio2 = 0.5;

							// Some optional scaling factor for weights
							if (rigidbodies1 || rigidbodies2)
							{
								float mass1 = 0;
								float mass2 = 0;

								if (rigidbodies1)
									mass1 = rigidbodies1[i].mass;

								if (rigidbodies2)
									mass2 = rigidbodies2[j].mass;

								float totalMass = mass1 + mass2;

								massRatio1 = mass1 / totalMass;
								massRatio2 = mass2 / totalMass;
							}

							displacement1 += info.mtv * massRatio1;
							displacement2 -= info.mtv * massRatio2;


							transforms1[i].pos += displacement1;
							collisionComp1.collider->MoveBy(displacement1);

							transforms2[j].pos += displacement2;
							collisionComp2.collider->MoveBy(displacement2);
						}
						if (info.overlap && (rigidbodies1 || rigidbodies2))
						{
							if (info.mtv.x == 0 && info.mtv.y == 0)
								continue;

							Rigidbody* rb1 = &rigidbodies1[i];
							Rigidbody* rb2 = &rigidbodies2[j];

							Kinematics* kinematics1 = GetColumn(arch1, Kinematics);
							Kinematics* kinematics2 = GetColumn(arch2, Kinematics);
								// We need to calculate the velocity at the time of collision and not at the time of overlaps

							Vector2 relativeVelVector = kinematics2[j].vel - kinematics1[i].vel;
							Vector2 normal = Vector::GetNormalized(info.mtv);
							//Vector2 normal = Vector::GetNormalized(info.mtv);

							float relativeVel = Vector::DotProduct(relativeVelVector, normal);

							if (relativeVel > 0)
							{
								//printf("relative vel: %f\n", relativeVel);
								float invMass1, invMass2;
								if (rb1)
									invMass1 = 1.0 / rb1->mass;
								else
									invMass1 = 0;

								if (rb2)
									invMass2 = 1.0 / rb2->mass;
								else
									invMass2 = 0;

								// Calculate impulse
								float impulseScalar = -(1 + (rb1->elasticity + rb2->elasticity) / 2) *
									relativeVel /
									(invMass1 + invMass2);
								Vector2 impulse = normal * impulseScalar;

								// Apply impulse to kinematics
								kinematics1[i].vel -= impulse * invMass1;
								kinematics2[j].vel += impulse * invMass2;

								relativeVelVector = kinematics2[j].vel - kinematics1[i].vel;

								relativeVel = Vector::DotProduct(relativeVelVector, normal);

								// Apply friction
								float sf = (rb1->staticFriction + rb2->staticFriction) / 2;
								float df = (rb1->dynamicFriction + rb2->dynamicFriction) / 2;

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
								kinematics1[i].vel -= frictionImpulse * invMass1;
								kinematics2[j].vel += frictionImpulse * invMass2;
							}
						}
					}
				}
			}
		}
	}
}
