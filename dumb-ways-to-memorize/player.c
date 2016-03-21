#include "globals.h"
#include "entity.h"
#include "player.h"

char *name = PLAYER_NAME;
void *gPlayer = NULL;
int gPlayerLives = 0;

void InitPlayer()
{
	gPlayer = (void*) InitNewEntity();
	memcpy(gPlayer,FindCachedEntity("Player"), sizeof(entity_t));
	gPlayerLives = PLAYER_LIVES;
	((entity_t*) gPlayer)->Draw = DrawGeneric;
	((entity_t*) gPlayer)->Think = ThinkPlayer;
	((entity_t*) gPlayer)->Touch = TouchPlayer;
	((entity_t*) gPlayer)->PowerUp = NULL;
}

entity_t *GetPlayer()
{
	return (entity_t*) gPlayer;
}

void DecrementPlayerLives()
{
	gPlayerLives--;
}