#include "Scoreboard.h"

void drawScoreboard()
{
	engSetColor(0xFFFFFFFF);

	const int xPos = 700;
	int yPos = 10;
	const int lineSpacing = 15;

	engTextf(xPos, yPos, "Score:");
	yPos += lineSpacing;

	for (auto& player : players)
	{
		if (!player.alive)
			continue;

		engTextf(xPos, yPos, "%s: %i", player.name, player.points);
		yPos += lineSpacing;
	}
}