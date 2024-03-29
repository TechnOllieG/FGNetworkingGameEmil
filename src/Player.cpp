#include "Player.h"
#include "Client.h"
#include "Engine.h"
#include "MessageType.h"
#include "Network.h"
#include <cmath>
#include "Projectile.h"

#define clamp(a, min, max) (a < min ? min : (a > max ? max : a))

Player players[PLAYER_MAX];
#if CLIENT
int possessedPlayerId = -1;
#endif

void Player::netReceivePosition(float newX, float newY)
{
	newX = clamp(newX, 0.f + playerRadius, 800.f - playerRadius);
	newY = clamp(newY, 0.f + playerRadius, 600.f - playerRadius);

	errorX = newX - x;
	errorY = newY - y;
}

void Player::spawn(int id, int spawnX, int spawnY)
{
	this->id = id;
	alive = true;
	x = (float) spawnX;
	y = (float) spawnY;
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
		int frameInputX = 0;
		int frameInputY = 0;

		if (engKeyDown(Key::A)) frameInputX -= 1;
		if (engKeyDown(Key::D)) frameInputX += 1;
		if (engKeyDown(Key::W)) frameInputY -= 1;
		if (engKeyDown(Key::S)) frameInputY += 1;

		if (frameInputX != inputX || frameInputY != inputY)
		{
			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerInput);
			msg.write<int>(id);
			msg.write<float>(x);
			msg.write<float>(y);

			msg.write<char>(frameInputX);
			msg.write<char>(frameInputY);

			clientSend(msg);
			msg.free();
		}

		inputX = frameInputX;
		inputY = frameInputY;

		if(engKeyPressed(Key::Space))
		{
			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerRequestFire);
			clientSend(msg);
			msg.free();
		}
	}
#endif

	if(!hasControl())
	{
		float errorDeltaX = errorX * playerErrorCorrectionStrength * engDeltaTime();
		float errorDeltaY = errorY * playerErrorCorrectionStrength * engDeltaTime();

		x += errorDeltaX;
		y += errorDeltaY;
		errorX -= errorDeltaX;
		errorY -= errorDeltaY;
	}

	x += inputX * playerSpeed * engDeltaTime();
	y += inputY * playerSpeed * engDeltaTime();

	x = clamp(x, 0.f + playerRadius, 800.f - playerRadius);
	y = clamp(y, 0.f + playerRadius, 600.f - playerRadius);
}

void Player::draw()
{
	engSetColor(0xDEADBEEF);
#if CLIENT
	if (hasControl())
		engSetColor(0xADDEBEEF);
#endif
	engFillRect((int)x - playerRadius, (int)y - playerRadius, 32, 32);
	engText((int)x - playerRadius, (int)y - playerRadius - 16, name);
}
