#pragma once

#include <vector>

#include "Vector.h"

enum class ShapeType
{
	Undefined,
	Triangle,
	Rectangle,
	ConvexPolygon,
	ConcavePolygon
};

// Collision methods ordered by efficiency meaning the more efficient one will be rated higher
enum class CollisionMethod
{
	SAT,
	AABB
};

enum class SolvingMethod
{
	Impulse,
};

// It uses something similair to euler angles however it has only one axis of rotation
struct Shape2D
{
	Shape2D() = default;

	virtual void MoveBy(const Vector2& addPosition) = 0;

	virtual void AddRotation(float addRotation) = 0;

	virtual void AddScale(const Vector2& addScale) = 0;

	virtual CollisionMethod GetCollisionMethod() = 0;

	// radians
	float angle;

	Vector2 center;

	// Rotation is clockwise

	void ApplyRotation(float angleRadians);

	// Calculate the delta of angles
	inline void SetRotation(float desiredAngle) { ApplyRotation(desiredAngle - angle); }

	virtual void UpdateSideVectors();

	// The first vertex must be the top left corner (for AABB's)
	std::vector<Vector2> vertices;

	std::vector<Vector2> sides;
};

struct Polygon : Shape2D // rewrite this to use operator overloading
{
	void MoveBy(const Vector2& addPosition) override;

	virtual void AddRotation(float addRotation) override;

	void AddScale(const Vector2& addScale) override {}

	virtual CollisionMethod GetCollisionMethod() override { return CollisionMethod::SAT; }
};

struct OBB : Polygon
{
	// Position is centered
	OBB(float width, float height, Vector2 position = { 0,0 }, float rotation = 0);
	float w, h;
	
	// We only need to check two sides for rectangles
	void UpdateSideVectors() override;
};

struct AABB : OBB
{
	// position is centered
	AABB(float width, float height, Vector2 position = { 0,0 });
	// top left corner
	float x, y;
	//float w, h;
	// AABB doesn't have rotation
	void AddRotation(float addRotation) override {}
	void AddScale(const Vector2& addScale) override
	{
		w *= addScale.x + 1;
		h *= addScale.y + 1;
	}

	CollisionMethod GetCollisionMethod() override { return CollisionMethod::AABB; }
};