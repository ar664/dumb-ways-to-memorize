#include "globals.h"
#include "entity.h"
#include "player.h"

char *name = PLAYER_NAME;
void *Player = NULL;
int PlayerLives = 0;

void InitPlayer()
{
	Player = (void*) InitNewEntity();
	PlayerLives = PLAYER_LIVES;
}

entity_t *GetPlayer()
{
	return (entity_t*) Player;
}

void DecrementPlayerLives()
{
	PlayerLives--;
}