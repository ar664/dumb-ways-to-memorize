#ifndef __PLAYER_H
#define __PLAYER_H

#include "entity.h"
#define PLAYER_NAME "Player1"
#define PLAYER_LIVES 3

void InitPlayer();
entity_t *GetPlayer();
void DecrementPlayerLives();

extern int gPlayerLives;
extern char *name;

#endif