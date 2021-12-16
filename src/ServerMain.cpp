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
#include "Library.h"
#include "PointEntity.h"
#include "Scoreboard.h"

#if SERVER

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
			Vector2 newPos = msg.read<Vector2>();
			player->netReceivePosition(newPos);

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
			Vector2 newPos = msg.read<Vector2>();
			player->netReceivePosition(newPos);

			player->inputVector = msg.read<Vector2>();
			serverBroadcast(msg);
			break;
		}

		case MessageType::PlayerFireButtonState:
		{
			Player* player = &players[userId];

			bool newState = msg.read<bool>();
			if (newState != player->currentlyFiring)
				player->currentlyFiring = newState;
			else
				engPrint("Client tried to change firing state to what it already is");

			break;
		}

		case MessageType::PlayerSprint:
		{
			int id = msg.read<int>();
			if (id != userId)
			{
				serverKickUser(userId);
				return;
			}
			bool sprintState = msg.read<bool>();
			players[userId].sprinting = sprintState;
			serverBroadcast(msg);
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
							spawnMsg.write<Vector2>(players[i].pos);

							serverSendTo(spawnMsg, event.userId);
							spawnMsg.free();
						}

						{
							NetMessage nameMsg;
							nameMsg.write<MessageType>(MessageType::PlayerName);
							nameMsg.write<int>(i);
							nameMsg.write<unsigned char>((unsigned char) strlen(players[i].name));
							nameMsg.write(players[i].name, (int) strlen(players[i].name));

							serverSendTo(nameMsg, event.userId);
							nameMsg.free();
						}

						{
							NetMessage pointMsg;
							pointMsg.write<MessageType>(MessageType::PlayerPoint);
							pointMsg.write<int>(i);
							pointMsg.write<int>(players[i].points);
							serverSendTo(pointMsg, event.userId);
							pointMsg.free();
						}
					}

					{
						Player* player = &players[event.userId];
						player->spawn(event.userId, Vector2((float) (rand() % 800), (float) (rand() % 600)));

						NetMessage msg;
						msg.write<MessageType>(MessageType::PlayerSpawn);
						msg.write<int>(event.userId);
						msg.write<Vector2>(player->pos);

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

		if(gameStarted)
		{
			float time = engElapsedTime();
			if (time - lastPointEntitySpawn > pointEntitySpawnCooldown)
			{
				lastPointEntitySpawn = time;

				int pointEntityIndex = -1;
				for (int i = 0; i < POINT_ENTITIES_MAX; i++)
				{
					if (pointEntities[i].alive)
						continue;

					pointEntityIndex = i;
					break;
				}

				if (pointEntityIndex != -1)
				{
					PointEntity* entity = &pointEntities[pointEntityIndex];
					entity->spawn(Vector2(rand() % 800, rand() % 600));

					NetMessage msg;
					msg.write<MessageType>(MessageType::PointEntitySpawn);
					msg.write<int>(pointEntityIndex);
					msg.write<Vector2>(entity->pos);
					serverBroadcast(msg);
					msg.free();
				}
				else
				{
					engPrint("Ran out of point entities");
				}
			}

			// Check if we have a winner
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
			{
				engTextf(400, 300, "'%s' WINS!", players[lastAlivePlayer].name);
				gameWon = true;
			}
			else if (numAlivePlayers == 0)
			{
				if (gameWon)
				{
					gameWon = false;
					gameStarted = false;
				}
				else
					engText(400, 300, "Draw :(");
			}
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

		for (auto& pointEntity : pointEntities)
		{
			if (pointEntity.alive)
				pointEntity.draw();
		}

		drawScoreboard();
	}

	return 0;
}

#endif
