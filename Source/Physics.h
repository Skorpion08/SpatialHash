#pragma once

#include "ECS.h"
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
	float staticFriction;
	float dynamicFriction;
};

struct AngularKinematics
{
	// nothing yet
};

struct Rigidbody
{
	float mass;
};

struct Gravity
{
	Vector2 gravityAcc;

};

inline COMPONENT(Transform)
inline COMPONENT(Kinematics)
inline COMPONENT(Friction)
inline COMPONENT(Rigidbody)
inline COMPONENT(Gravity)

class Physics
{
public:
	void UpdateTransform(double deltaTime);
};