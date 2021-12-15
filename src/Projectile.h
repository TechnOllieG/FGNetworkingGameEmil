#pragma once
#define PROJECTILE_MAX 256
#include "Library.h"

const float projectileSpeed = 400.f;
const float despawnDelay = 5.f;
const float projectileRadius = 4.f;
const float playerFireCooldown = 1.f;

class Projectile
{
public:
	void spawn(int player, Vector2 spawnPos, Vector2 dir);
	void destroy();
	void update();
	void draw();

	bool alive = false;

	Vector2 pos;
	Vector2 velocity;

	float spawnTime = 0.f;
	int ownerPlayer = -1;
};
extern Projectile projectiles[PROJECTILE_MAX];