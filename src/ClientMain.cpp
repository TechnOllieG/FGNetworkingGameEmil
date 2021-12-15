#include "Engine.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include "Player.h"
#include "Client.h"
#include "Network.h"
#include "MessageType.h"
#include "Projectile.h"

#if CLIENT

void handleMessage(NetMessage msg)
{
	MessageType type = msg.read<MessageType>();

	switch(type)
	{
		case MessageType::PlayerSpawn:
		{
			int id = msg.read<int>();
			Vector2 pos = msg.read<Vector2>();
			players[id].spawn(id, pos);
			break;
		}

		case MessageType::PlayerName:
		{
			int id = msg.read<int>();
			int nameLen = msg.read<unsigned char>();

			Player* player = &players[id];

			msg.read(player->name, nameLen);
			player->name[nameLen] = 0;
			break;
		}

		case MessageType::PlayerDestroy:
		{
			int id = msg.read<int>();
			players[id].destroy();
			break;
		}

		case MessageType::PlayerPossess:
		{
			possessedPlayerId = msg.read<int>();

			static const char* myName = "KillerMan";
			NetMessage nameMsg;
			nameMsg.write<MessageType>(MessageType::PlayerName);
			nameMsg.write<int>(possessedPlayerId);
			nameMsg.write<unsigned char>((unsigned char) strlen(myName));
			nameMsg.write(myName, (int) strlen(myName));

			clientSend(nameMsg);
			nameMsg.free();
			break;
		}

		case MessageType::PlayerPosition:
		{
			int id = msg.read<int>();
			Player* player = &players[id];
			if (player->hasControl())
				break;

			player->pos = msg.read<Vector2>();
			break;
		}

		case MessageType::PlayerInput:
		{
			int id = msg.read<int>();
			Player* player = &players[id];

			if (player->hasControl())
				break;

			Vector2 newPos = msg.read<Vector2>();

			player->netReceivePosition(newPos);

			player->inputVector = msg.read<Vector2>();
			break;
		}

		case MessageType::ProjectileSpawn:
		{
			int id = msg.read<int>();
			int playerId = msg.read<int>();
			Vector2 pos = msg.read<Vector2>();
			Vector2 dir = msg.read<Vector2>();

			projectiles[id].spawn(playerId, pos, dir);
			break;
		}
	}
}

int WinMain(HINSTANCE, HINSTANCE, char*, int)
{
	engInit();
	netInit();

	/*if (!clientConnect("10.20.2.178", 666))
		return 1;*/

	if (!clientConnect("127.0.0.1", 666))
		return 1;

	while(engBeginFrame() && clientIsConnected())
	{
		NetEvent event;
		while(netPollEvent(&event))
		{
			switch(event.type)
			{
				case NetEventType::Message:
					//engPrint("Received %d bytes", event.message.size);
					handleMessage(event.message);
					break;
			}

			event.free();
		}

		engSetColor(0x4444CCFF);
		engClear();

		for(auto& player : players)
		{
			if (player.alive)
				player.update();
		}

		for (auto& projectile : projectiles)
		{
			if (projectile.alive)
				projectile.update();
		}

		for(auto& player : players)
		{
			if(player.alive)
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
