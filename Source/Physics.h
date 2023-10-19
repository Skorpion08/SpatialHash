#pragma once

#include <memory>

#include "ECS.h"
#include "Shape.h"
#include "Vector.h"


#define PI 3.141592653589793

using Position = Vector2;
using Rotation = float;
using Scale = Vector2;

struct Transform
{
	Position pos = {0, 0};
	Rotation rotation = 0;
	Scale scale = {0, 0};
};


struct Kinematics
{
	Vector2 vel = {0, 0};
	Vector2 acc = { 0, 0 };
};

struct Friction
{

};

struct AngularKinematics
{
	// nothing yet
};

struct Rigidbody
{
	float mass = 10;

	float elasticity = 1;

	float staticFriction = 0.5;
	float dynamicFriction = 0.3;

	float linearDrag = 0.01;
};

struct Gravity
{
	Vector2 gravityAcc;

};

struct Collision
{
	Collision() = default;
	Collision(Shape2D* _collider, bool _blockCollision = true) : collider(_collider), blockCollision(_blockCollision) {}

	std::unique_ptr<Shape2D> collider;
	bool blockCollision = true;
};

inline COMPONENT(Transform)
inline COMPONENT(Kinematics)
inline COMPONENT(Friction)
inline COMPONENT(Rigidbody)
inline COMPONENT(Gravity)
inline COMPONENT(Collision)


struct CollisionInfo
{
	bool overlap;
	Vector2 mtv;
};

class Physics
{
public:
	void UpdateTransform(double deltaTime);
	void UpdateColliders();

	CollisionInfo AABBtoAABB(AABB& a, AABB& b);
	CollisionInfo POLYtoPOLY(Shape2D& shapeA, Shape2D& shapeB);

	void FindSolveCollisions();
};