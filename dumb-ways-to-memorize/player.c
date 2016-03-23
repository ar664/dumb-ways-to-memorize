#include "globals.h"
#include "parsepowerup.h"
#include "entity.h"
#include "player.h"
#include "parselevel.h"
#include <stdio.h>

char *name = PLAYER_NAME;
entity_t *gPlayer = NULL;
int gPlayerLives = 0;

void InitPlayer()
{
	gPlayer = InitNewEntity();
	if(!gPlayer)
	{
		printf("Could not Init Player");
		exitRequest = 1;
		return;
	}
	memcpy(gPlayer,FindCachedEntity("Player"), sizeof(entity_t));
	gPlayerLives = PLAYER_LIVES;
	gPlayer->Draw = DrawGeneric;
	gPlayer->Think = ThinkPlayer;
	gPlayer->Touch = TouchPlayer;
	gPlayer->PowerUp = gPowerUps ? UsePower : NULL;
	gPlayer->mPosition = gCurrentLevel ? gCurrentLevel->mSpawnPoint : gZeroPos;
	gPlayer->mWeight = 1;
}

entity_t *GetPlayer()
{
	return (entity_t*) gPlayer;
}

void DecrementPlayerLives()
{
	gPlayerLives--;
}