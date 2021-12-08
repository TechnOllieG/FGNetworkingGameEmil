#pragma once
#define PLAYER_MAX 20
#define PLAYER_NAME_MAX 15

const float playerSpeed = 100.f;
const float playerErrorCorrectionStrength = 1.5f;

class Player
{
public:
	int id = -1;
	bool alive = false;
	float x;
	float y;

	int inputX = 0;
	int inputY = 0;

	float errorX = 0.f;
	float errorY = 0.f;

	char name[PLAYER_NAME_MAX + 1];

	void netReceivePosition(float newX, float newY);

	void spawn(int id, int spawnX, int spawnY);
	void destroy();

	bool hasControl();
	void update();

	void draw();
};

extern Player players[PLAYER_MAX];

#if CLIENT
extern int possessedPlayerId;
#endif