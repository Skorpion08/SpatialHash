#pragma once

#include "Vector.h"

// It uses something similair to euler angles however it has only one axis of rotation
struct Shape2D
{
	Shape2D() : angleRad(0){}

	// Clockwise
	float angleRad;

	Point2 center;

	std::vector<Vector2> vertices;

	std::vector<Vector2> sides;

	// Rotation is clockwise

	//void ApplyRotation(float angleRadians);
	void ApplyRotation(float angleRadians);

	// Deprecated now
	std::vector<Vector2>& GetSideVectors_DEPRECATED();

	void UpdateSideVectors();
};