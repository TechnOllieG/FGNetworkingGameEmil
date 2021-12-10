#pragma once
#define PROJECTILE_MAX 256

const float projectileSpeed = 0.2;
const float despawnDelay = 5.f;

class Projectile
{
public:
	void spawn(float spawnX, float spawnY, int dirX, int dirY);
	void destroy();
	void update();
	void draw();

	bool alive = false;

	float x;
	float y;

	float velocityX;
	float velocityY;

	float spawnTime = 0.f;
};
extern Projectile projectiles[PROJECTILE_MAX];