#include "Engine.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include "Player.h"
#include "Server.h"
#include "Network.h"
#include "MessageType.h"
#include "Projectile.h"
#if SERVER

bool gameStarted = false;

void handleMessage(int userId, NetMessage msg)
{
	MessageType type = msg.read<MessageType>();
	switch(type)
	{
		case MessageType::PlayerName:
		{
			int playerId = msg.read<int>();
			if (playerId != userId)
			{
				serverKickUser(userId);
				break;
			}

			int nameLen = msg.read<unsigned char>();
			if(nameLen > PLAYER_NAME_MAX)
			{
				serverKickUser(userId);
				break;
			}

			Player* player = &players[userId];
			msg.read(player->name, nameLen);

			player->name[nameLen] = 0;

			serverBroadcast(msg);
			break;
		}

		case MessageType::PlayerPosition:
		{
			int playerId = msg.read<int>();
			if(playerId != userId)
			{
				serverKickUser(userId);
				break;
			}

			Player* player = &players[userId];
			float newX = msg.read<float>();
			float newY = msg.read<float>();
			player->netReceivePosition(newX, newY);

			serverBroadcast(msg);
			break;
		}

		case MessageType::PlayerInput:
		{
			int playerId = msg.read<int>();
			if (playerId != userId)
			{
				serverKickUser(userId);
				break;
			}

			Player* player = &players[userId];
			float newX = msg.read<float>();
			float newY = msg.read<float>();
			player->netReceivePosition(newX, newY);

			player->inputX = msg.read<char>();
			player->inputY = msg.read<char>();
			serverBroadcast(msg);
			break;
		}

		case MessageType::PlayerRequestFire:
		{
			if (!gameStarted)
				break;

			int projectileIndex = -1;
			for(int i = 0; i < PROJECTILE_MAX; i++)
			{
				if (projectiles[i].alive)
					continue;

				projectileIndex = i;
				break;
			}

			if(projectileIndex == -1)
			{
				engError("Ran out of projectiles");
				break;
			}

			Player* player = &players[userId];

			if (player->inputX == 0 && player->inputY == 0)
				break;

			if (engElapsedTime() - player->lastFireTime < playerFireCooldown)
				break;

			player->lastFireTime = engElapsedTime();
			
			projectiles[projectileIndex].spawn(userId, player->x, player->y, player->inputX, player->inputY);

			NetMessage response;
			response.write<MessageType>(MessageType::ProjectileSpawn);
			response.write<int>(projectileIndex);
			response.write<int>(userId);
			response.write<float>(player->x);
			response.write<float>(player->y);
			response.write<char>(player->inputX);
			response.write<char>(player->inputY);

			serverBroadcast(response);
			response.free();
			break;
		}
	}
}

int WinMain(HINSTANCE, HINSTANCE, char*, int)
{
	engInit();
	netInit();

	if (!serverStartup(666))
		return 1;

	while(engBeginFrame())
	{
		NetEvent event;
		while(netPollEvent(&event))
		{
			switch(event.type)
			{
				case NetEventType::UserConnected:
				{
					if (gameStarted)
					{
						serverKickUser(event.userId);
						break;
					}

					engPrint("User %d connected", event.userId);
					serverAcceptUser(event.userId);

					for (int i = 0; i < PLAYER_MAX; i++)
					{
						if (!players[i].alive)
							continue;

						{
							NetMessage spawnMsg;
							spawnMsg.write<MessageType>(MessageType::PlayerSpawn);
							spawnMsg.write<int>(i);
							spawnMsg.write<float>(players[i].x);
							spawnMsg.write<float>(players[i].y);

							serverSendTo(spawnMsg, event.userId);
							spawnMsg.free();
						}
						{
							NetMessage nameMsg;
							nameMsg.write<MessageType>(MessageType::PlayerName);
							nameMsg.write<int>(i);
							nameMsg.write<unsigned char>(strlen(players[i].name));
							nameMsg.write(players[i].name, strlen(players[i].name));

							serverSendTo(nameMsg, event.userId);
							nameMsg.free();
						}
					}

					{
						Player* player = &players[event.userId];
						player->spawn(event.userId, rand() % 800, rand() % 600);

						NetMessage msg;
						msg.write<MessageType>(MessageType::PlayerSpawn);
						msg.write<int>(event.userId);
						msg.write<float>(player->x);
						msg.write<float>(player->y);

						serverBroadcast(msg);
						msg.free();
					}

					{
						NetMessage msg;
						msg.write<MessageType>(MessageType::PlayerPossess);
						msg.write<int>(event.userId);

						serverSendTo(msg, event.userId);
						msg.free();
					}
					break;
				}

				case NetEventType::UserDisconnected:
				{
					engPrint("User %d disconnected", event.userId);
					players[event.userId].destroy();

					NetMessage destroyMsg;
					destroyMsg.write<MessageType>(MessageType::PlayerDestroy);
					destroyMsg.write<int>(event.userId);

					serverBroadcast(destroyMsg);
					destroyMsg.free();
					break;
				}

				case NetEventType::Message:
				{
					handleMessage(event.userId, event.message);
					break;
				}
			}

			event.free();
		}

		engSetColor(0xCC4444FF);
		engClear();

		if (engKeyPressed(Key::Space))
			gameStarted = !gameStarted;

		engSetColor(0xFFFFFFFF);
		engText(400, 12, gameStarted ? "STARTED" : "WAITING...");

		// Check if we have a winner
		if(gameStarted)
		{
			int numAlivePlayers = 0;
			int lastAlivePlayer = -1;
			for (auto& player : players)
			{
				if (player.alive)
				{
					++numAlivePlayers;
					lastAlivePlayer = player.id;
				}
			}

			if (numAlivePlayers == 1)
				engTextf(400, 300, "'%s' WINS!", players[lastAlivePlayer].name);
			else if (numAlivePlayers == 0)
				engText(400, 300, "Draw :(");
		}
		

		for (auto& player : players)
		{
			if (player.alive)
				player.update();
		}

		for (auto& projectile : projectiles)
		{
			if (projectile.alive)
				projectile.update();
		}

		for (auto& player : players)
		{
			if (player.alive)
				player.draw();
		}

		for (auto& projectile : projectiles)
		{
			if (projectile.alive)
				projectile.draw();
		}
	}

	return 0;
}

#endif
