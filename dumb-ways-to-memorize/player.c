#include "globals.h"
#include "entity.h"
#include "player.h"

char *name = PLAYER_NAME;
void *gPlayer = NULL;
int PlayerLives = 0;

void InitPlayer()
{
	gPlayer = (void*) InitNewEntity();
	PlayerLives = PLAYER_LIVES;
}

entity_t *GetPlayer()
{
	return (entity_t*) gPlayer;
}

void DecrementPlayerLives()
{
	PlayerLives--;
}