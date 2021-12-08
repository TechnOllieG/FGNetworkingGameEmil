#pragma once

enum class MessageType : unsigned char
{
	PlayerSpawn,
	PlayerName,
	PlayerDestroy,
	PlayerPossess,
	PlayerPosition,
	PlayerInput
};