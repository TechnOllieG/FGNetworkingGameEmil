#pragma once
#include <cmath>

#define EQUALITY_MARGIN 1.e-4f

struct Vector2
{
	float x = 0.f;
	float y = 0.f;

	Vector2()
	{
		x = 0.f;
		y = 0.f;
	}

	Vector2(float inBoth)
	{
		x = inBoth;
		y = inBoth;
	}

	Vector2(float inX, float inY)
	{
		x = inX;
		y = inY;
	}

	Vector2 operator*(float a)
	{
		return Vector2(x * a, y * a);
	}

	Vector2 operator/(float a)
	{
		return Vector2(x / a, y / a);
	}

	Vector2 operator+(Vector2 rhs)
	{
		return Vector2(x + rhs.x, y + rhs.y);
	}

	void operator+=(Vector2 rhs)
	{
		*this = Vector2(x + rhs.x, y + rhs.y);
	}

	Vector2 operator-(Vector2 rhs)
	{
		return Vector2(x - rhs.x, y - rhs.y);
	}

	void operator-=(Vector2 rhs)
	{
		*this = Vector2(x - rhs.x, y - rhs.y);
	}

	bool operator==(Vector2 rhs)
	{
		return abs(x - rhs.x) < EQUALITY_MARGIN && abs(y - rhs.y) < EQUALITY_MARGIN;
	}

	bool operator!=(Vector2 rhs)
	{
		return !(*this == rhs);
	}

	float sqrMagnitude()
	{
		return x * x + y * y;
	}

	float magnitude()
	{
		return sqrt(sqrMagnitude());
	}

	Vector2 normalized()
	{
		float mag = magnitude();
		return *this / mag;
	}

	void Normalize()
	{
		*this = normalized();
	}

	float static Distance(Vector2 a, Vector2 b)
	{
		return (a - b).magnitude();
	}

	float static SqrDistance(Vector2 a, Vector2 b)
	{
		return (a - b).sqrMagnitude();
	}
};