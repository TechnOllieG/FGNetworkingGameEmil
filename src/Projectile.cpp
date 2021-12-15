#include "Projectile.h"
#include "Engine.h"
#include "Player.h"
#include "Server.h"

Projectile projectiles[PROJECTILE_MAX];

void Projectile::spawn(int player, Vector2 spawnPos, Vector2 dir)
{
	alive = true;
	pos = spawnPos;

	ownerPlayer = player;

	velocity = dir * projectileSpeed;

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

	pos += velocity * engDeltaTime();

	for(auto& player : players)
	{
		if (!player.alive)
			continue;

		if (player.id == ownerPlayer)
			continue;

		float distSqr = (pos - player.pos).sqrMagnitude();
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
	engFillRect((int)(pos.x - projectileRadius), (int)(pos.y - projectileRadius), 8, 8);
}

