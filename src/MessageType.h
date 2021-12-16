#pragma once

enum class MessageType : unsigned char
{
	PlayerSpawn,
	PlayerName,
	PlayerDestroy,
	PlayerPossess,
	PlayerPosition,
	PlayerInput,
	ProjectileSpawn,
	PlayerFireButtonState,
	PointEntitySpawn,
	PointEntityPickup,
	PlayerAcceleration,
	PlayerPoint,
	PlayerSprint
};