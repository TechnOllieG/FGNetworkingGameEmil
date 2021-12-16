#pragma once
#define PLAYER_MAX 20
#define PLAYER_NAME_MAX 15
#include "Library.h"

const float playerSpeed = 180.f;
const float playerSprintSpeed = 300.f;
const float playerErrorCorrectionStrength = 1.5f;
const float playerRadius = 16;
const float playerPushbackForce = 10;
const float playerPushbackFriction = 1.2f;

class Player
{
public:
	int id = -1;
	int points = 0;
	bool alive = false;
	bool sprinting = false;
	Vector2 pos;

	Vector2 inputVector;
	Vector2 errorVector;
	Vector2 additionalVelocity;

	float lastFireTime = 0.f;
	bool currentlyFiring = false;

	char name[PLAYER_NAME_MAX + 1] {0};

	void netReceivePosition(Vector2 newPos);

	void spawn(int id, Vector2 spawnPos);
	void destroy();

	bool hasControl();
	void update();

	void draw();
};

extern Player players[PLAYER_MAX];

#if CLIENT
extern int possessedPlayerId;
#endif