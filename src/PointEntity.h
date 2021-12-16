#pragma once
#include "Library.h"

#define POINT_ENTITIES_MAX 128

const float pointEntityRadius = 4.f;
const float pointEntitySpawnCooldown = 1.f;

class PointEntity
{
public:
	void spawn(Vector2 spawnPos);
	void destroy();
	void draw();

	bool alive = false;
	Vector2 pos;
};

extern PointEntity pointEntities[POINT_ENTITIES_MAX];
extern float lastPointEntitySpawn;