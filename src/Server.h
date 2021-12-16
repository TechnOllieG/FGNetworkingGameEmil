#pragma once
#include <WinSock2.h>
struct NetMessage;

#define SERVER_CAPACITY 128

bool serverStartup(unsigned short port);
bool serverIsStarted();

void serverAcceptUser(int userId);
void serverKickUser(int userId);

void serverSendTo(const NetMessage& message, int userId);
void serverBroadcast(const NetMessage& message);

extern bool gameStarted;
extern bool gameWon;