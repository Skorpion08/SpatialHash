#pragma once

#include "Vector.h"

// It uses something similair to euler angles however it has only one axis of rotation
struct Shape2D
{
	Shape2D() : angleRad(0){}

	// Counter clockwise
	float angleRad;

	Point2 center;

	std::vector<Vector2> vertices;

	// Rotation is clockwise

	//void ApplyRotation(float angleRadians);
	void ApplyRotation(float angleRadians);


	void GetSideVectors(std::vector<Vector2>& outSides);
};