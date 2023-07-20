#pragma once

#include <cmath>

struct Point2
{
	Point2() : x(0), y(0) {}
	Point2(float p_x, float p_y) : x(p_x), y(p_y) {}
	float x, y;
};


struct Vector2
{
	Vector2() : x(0), y(0) {}
	Vector2(float p_x, float p_y) : x(p_x), y(p_y) {}
	Vector2(Point2 a, Point2 b) : x(b.x-a.x), y(b.y-b.y) {}

	float x, y;

	inline Vector2 GetNormal()
	{
		return Vector2(-y, x);
	}
	inline void Normalize()
	{
		float magnitude = this->GetMagnitude();
		x /= magnitude;
		y /= magnitude;
	}
	inline float GetMagnitude()
	{
		return sqrtf(x * x + y * y);
	}
};

namespace Vector
{
inline float DotProduct(Vector2 vec1, Vector2 vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}
}