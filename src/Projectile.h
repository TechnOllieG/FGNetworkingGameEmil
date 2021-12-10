#pragma once
#define PROJECTILE_MAX 256

const float projectileSpeed = 400.f;
const float despawnDelay = 5.f;
const float projectileRadius = 4.f;
const float playerFireCooldown = 1.f;

class Projectile
{
public:
	void spawn(int player, float spawnX, float spawnY, int dirX, int dirY);
	void destroy();
	void update();
	void draw();

	bool alive = false;

	float x;
	float y;

	float velocityX;
	float velocityY;

	float spawnTime = 0.f;
	int ownerPlayer;
};
extern Projectile projectiles[PROJECTILE_MAX];