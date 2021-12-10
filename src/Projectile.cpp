#include "Projectile.h"
#include "Engine.h"

Projectile projectiles[PROJECTILE_MAX];

void Projectile::spawn(float spawnX, float spawnY, int dirX, int dirY)
{
	alive = true;
	x = spawnX;
	y = spawnY;

	velocityX = dirX * projectileSpeed;
	velocityY = dirY * projectileSpeed;

	spawnTime = engElapsedTime();
}

void Projectile::destroy()
{
	alive = false;
}

void Projectile::update()
{
	x += velocityX * engDeltaTime();
	y += velocityY * engDeltaTime();

	if(engElapsedTime() - spawnTime > despawnDelay)
	{
		destroy();
		return;
	}
}

void Projectile::draw()
{
	engSetColor(0x000000FF);
	engFillRect((int)x, (int)y, 8, 8);
}

