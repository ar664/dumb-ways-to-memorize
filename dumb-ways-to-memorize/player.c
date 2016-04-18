#include "globals.h"
#include "parsepowerup.h"
#include "entity.h"
#include "player.h"
#include "parselevel.h"
#include <stdio.h>
#include "dumb_physics.h"

char *gPlayerName = PLAYER_NAME;
entity_t *gPlayer = NULL;
int gPlayerLives = 0;

void InitPlayer()
{
	cpVect *cpPos;
	if(!gPlayer)
	{
		gPlayer = InitNewEntity();
	}

	if(!gPlayer)
	{
		printf("Could not Init Player");
		exitRequest = 1;
		return;
	}
	memcpy(gPlayer,FindCachedEntity("Player"), sizeof(entity_t));
	if(!gPlayerLives)
	{
		PLAYER_LIVES;
	}
	gPlayer->Draw = DrawGeneric;
	gPlayer->Think = ThinkPlayer;
	gPlayer->Touch = TouchPlayer;
	gPlayer->PowerUp = gPowerUps ? UsePower : NULL;
	cpPos = gCurrentLevel ? (cpVect*) Vec2Cp(&gCurrentLevel->mSpawnPoint) : (cpVect*)&cpvzero;
	if(cpPos)
	{
		cpBodySetPos(gPlayer->mPhysicsProperties->body, *cpPos);
	}
	if(gCurrentLevel && cpPos)
	{
		free(cpPos);
	}

	gPlayer->mHealth = 100;
	gPlayer->mNextThink = gCurrentTime + 2*UPDATE_FRAME_DELAY;
	AddEntityToPhysics(gPlayer);
}

void DecrementPlayerLives()
{
	gPlayerLives--;
}