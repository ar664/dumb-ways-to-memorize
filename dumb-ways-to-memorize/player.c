#include "globals.h"
#include "parsepowerup.h"
#include "entity.h"
#include "player.h"
#include "parselevel.h"
#include <stdio.h>
#include "dumb_physics.h"

char *gPlayerName = PLAYER_NAME;
entity_t *gPlayer = NULL;
entity_t *gCursor = NULL;
int gPlayerLives = 3;

void InitPlayer()
{
	cpVect *cpPos;
	entity_t *ent;
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
	ent = FindCachedEntity(PLAYER_STR);
	if(!ent)
	{
		printf("Could not Init Player");
		exitRequest = 1;
		return;
	}
	memcpy(gPlayer, ent, sizeof(entity_t));
	if(gPlayerLives < 1)
	{
		gPlayerLives = PLAYER_LIVES;
	}
	gPlayer->Draw = DrawGeneric;
	gPlayer->Think = ThinkPlayer;
	gPlayer->Touch = TouchPlayer;
	gPlayer->PowerUp = gPowerUps ? UsePower : NULL;
	cpPos = gCurrentLevel ? (cpVect*) Vec2Cp(&gCurrentLevel->mSpawnPoint) : (cpVect*)&cpvzero;

	AddPhyicsToEntity(gPlayer);
	
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
	AddCollisionHandlerToEntity(gPlayer);
	
}

void InitCursor()
{
	entity_t *ent;
	if(!gCursor)
	{
		gCursor = InitNewEntity();
	}
		

	if(!gCursor)
	{
		printf("Could not Init Cursor");
		exitRequest = 1;
		return;
	}

	SDL_ShowCursor(false);
	ent = FindCachedEntity(CURSOR_STR);
	if(!ent)
	{
		return;
	}
	memcpy(gCursor, ent, sizeof(entity_t));

	cpBodySetPos(gCursor->mPhysicsProperties->body, cpvzero);
	gCursor->Think = ThinkCursor;
	gCursor->Draw = DrawGeneric;
	gCursor->Touch = NULL;
	gCursor->mNextThink = gCurrentTime + UPDATE_FRAME_DELAY;

}