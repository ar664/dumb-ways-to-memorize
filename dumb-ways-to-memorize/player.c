#include "globals.h"
#include "parsepowerup.h"
#include "entity.h"
#include "player.h"

char *name = PLAYER_NAME;
entity_t *gPlayer = NULL;
int gPlayerLives = 0;

void InitPlayer()
{
	gPlayer = InitNewEntity();
	memcpy(gPlayer,FindCachedEntity("Player"), sizeof(entity_t));
	gPlayerLives = PLAYER_LIVES;
	((entity_t*) gPlayer)->Draw = DrawGeneric;
	((entity_t*) gPlayer)->Think = ThinkPlayer;
	((entity_t*) gPlayer)->Touch = TouchPlayer;
	((entity_t*) gPlayer)->PowerUp = gPowerUps ? UsePower : NULL;
}

entity_t *GetPlayer()
{
	return (entity_t*) gPlayer;
}

void DecrementPlayerLives()
{
	gPlayerLives--;
}