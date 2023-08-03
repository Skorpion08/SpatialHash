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
	Vector2(Vector2 a, Vector2 b) : x(b.x-a.x), y(b.y-a.y) {}

	float x, y;

	inline Vector2 GetNormal()
	{
		return Vector2(-y, x).Normalized();
	}
	inline Vector2 Normalized()
	{
		float magnitude = this->GetMagnitude();
		x /= magnitude;
		y /= magnitude;
		return *this;
	}
	inline float GetMagnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	inline Vector2 operator -()
	{
		return Vector2(-this->x, -this->y);
	}
};

inline Vector2 operator + (const Vector2& a, const Vector2& b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

inline Vector2 operator - (const Vector2& a, const Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

namespace Vector
{
inline float DotProduct(Vector2 vec1, Vector2 vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}
inline Vector2 GetNormal(const Vector2& vector)
{
	return Vector2(-vector.y, vector.x);
}
inline Vector2 GetNormalized(const Vector2& vector)
{
	float magnitude = vector.GetMagnitude();
	return Vector2(vector.x / magnitude, vector.y / magnitude);
}

}