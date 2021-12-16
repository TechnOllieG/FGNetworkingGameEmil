#include "PointEntity.h"
#include "Engine.h"
#include "Player.h"

PointEntity pointEntities[POINT_ENTITIES_MAX];
float lastPointEntitySpawn = -100.f;

void PointEntity::spawn(Vector2 spawnPos)
{
	alive = true;
	pos = spawnPos;
}

void PointEntity::destroy()
{
	alive = false;
}

void PointEntity::draw()
{
	engSetColor(0x00FF00FF);
	engFillRect(pos.x - pointEntityRadius, pos.y - pointEntityRadius, pointEntityRadius * 2.f, pointEntityRadius * 2.f);
}