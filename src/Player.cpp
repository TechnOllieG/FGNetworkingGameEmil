#include "Player.h"
#include "Client.h"
#include "Engine.h"
#include "MessageType.h"
#include "Network.h"
#include <cmath>
#include "Projectile.h"
#include "Engine.h"
#include "Server.h"
#include "Library.h"

float clamp(float a, float min, float max)
{
	return (a < min ? min : (a > max ? max : a));
}

Vector2 clamp(Vector2 a, Vector2 min, Vector2 max)
{
	return Vector2(clamp(a.x, min.x, max.x), clamp(a.y, min.y, max.y));
}

Player players[PLAYER_MAX];
#if CLIENT
int possessedPlayerId = -1;
#endif

void Player::netReceivePosition(Vector2 newPos)
{
	newPos = clamp(newPos, Vector2(0.f + playerRadius),
		Vector2(800.f - playerRadius, 600.f - playerRadius));
	
	errorVector = Vector2(newPos.x - pos.x, newPos.y - pos.y);
}

void Player::spawn(int id, Vector2 spawnPos)
{
	this->id = id;
	alive = true;
	pos = spawnPos;
}

void Player::destroy()
{
	alive = false;
}

bool Player::hasControl()
{
#if SERVER
	return false;
#else
	return id == possessedPlayerId;
#endif
}

void Player::update()
{
#if CLIENT
	if(hasControl())
	{
		Vector2 frameInput;

		if (engKeyDown(Key::A)) frameInput.x -= 1;
		if (engKeyDown(Key::D)) frameInput.x += 1;
		if (engKeyDown(Key::W)) frameInput.y -= 1;
		if (engKeyDown(Key::S)) frameInput.y += 1;

		float mag = frameInput.sqrMagnitude();

		if (mag > 0.000001f || mag < -0.000001f)
		{
			frameInput.Normalize();
			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerInput);
			msg.write<int>(id);
			msg.write<Vector2>(pos);

			msg.write<Vector2>(frameInput);

			clientSend(msg);
			msg.free();
		}

		inputVector = frameInput;

		if (currentlyFiring ? !engKeyDown(Key::Space) : engKeyDown(Key::Space))
		{
			currentlyFiring = !currentlyFiring;
			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerFireButtonState);
			msg.write<bool>(currentlyFiring);
			clientSend(msg);
			msg.free();
		}
	}
#endif

	if(!hasControl())
	{
		if (gameStarted && currentlyFiring && engElapsedTime() - lastFireTime > playerFireCooldown)
		{
			int projectileIndex = -1;
			for (int i = 0; i < PROJECTILE_MAX; i++)
			{
				if (projectiles[i].alive)
					continue;

				projectileIndex = i;
				break;
			}

			if (projectileIndex != -1)
			{
				float mag = inputVector.sqrMagnitude();
				if (mag > 0.000001f || mag < -0.000001f)
				{
					lastFireTime = engElapsedTime();

					projectiles[projectileIndex].spawn(id, pos, inputVector);

					NetMessage response;
					response.write<MessageType>(MessageType::ProjectileSpawn);
					response.write<int>(projectileIndex);
					response.write<int>(id);
					response.write<Vector2>(pos);
					response.write<Vector2>(inputVector);

					serverBroadcast(response);
					response.free();
				}
			}
			else
			{
				engError("Ran out of projectiles");
			}
		}

		Vector2 errorDelta = errorVector * (playerErrorCorrectionStrength * engDeltaTime());

		pos += errorDelta;
		errorVector -= errorDelta;
	}

	pos += inputVector * (playerSpeed * engDeltaTime());

	pos = clamp(pos, Vector2(0.f + playerRadius),
		Vector2(800.f - playerRadius, 600.f - playerRadius));
}

void Player::draw()
{
	engSetColor(0xDEADBEEF);
#if CLIENT
	if (hasControl())
		engSetColor(0xADDEBEEF);
#endif
	engFillRect((int)(pos.x - playerRadius), (int)(pos.y - playerRadius), 32, 32);
	engText((int)(pos.x - playerRadius), (int)(pos.y - playerRadius) - 16, name);
}
