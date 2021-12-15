#pragma once
#include <cmath>

struct Vector2
{
	float x;
	float y;

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

	Vector2 operator-(Vector2 rhs)
	{
		return Vector2(x - rhs.x, y - rhs.y);
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
};