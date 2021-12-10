#include "Projectile.h"
#include "Engine.h"
#include "Player.h"
#include "Server.h"

Projectile projectiles[PROJECTILE_MAX];

void Projectile::spawn(int player, float spawnX, float spawnY, int dirX, int dirY)
{
	alive = true;
	x = spawnX;
	y = spawnY;

	ownerPlayer = player;

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
	if (engElapsedTime() - spawnTime > despawnDelay)
	{
		destroy();
		return;
	}

	x += velocityX * engDeltaTime();
	y += velocityY * engDeltaTime();

	for(auto& player : players)
	{
		if (!player.alive)
			continue;

		if (player.id == ownerPlayer)
			continue;

		float diffX = x - player.x;
		float diffY = y - player.y;
		float distSqr = (diffX * diffX + diffY * diffY);
		float radiusSqr = projectileRadius + playerRadius;
		radiusSqr = radiusSqr * radiusSqr;

		if(distSqr < radiusSqr)
		{
			destroy();

#if SERVER
			serverKickUser(player.id);
			engPrint("'%s' --> '%s'", players[ownerPlayer].name, player.name);
#endif
			return;
		}
	}
}

void Projectile::draw()
{
	engSetColor(0x000000FF);
	engFillRect((int)x - projectileRadius, (int)y - projectileRadius, 8, 8);
}

