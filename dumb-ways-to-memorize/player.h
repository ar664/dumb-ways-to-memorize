#ifndef __PLAYER_H
#define __PLAYER_H

#include "entity.h"
#define PLAYER_NAME "Player1"
#define PLAYER_LIVES 3
#define PLAYER_BASE_JUMP	-5
#define PLAYER_BASE_SPEED	5


void InitPlayer();
entity_t *GetPlayer();
void DecrementPlayerLives();

extern int gPlayerLives;
extern entity_t *gPlayer;
extern char *name;

#endif