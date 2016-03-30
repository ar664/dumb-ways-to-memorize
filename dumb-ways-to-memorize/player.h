#ifndef __PLAYER_H
#define __PLAYER_H

#include "entity.h"
#define PLAYER_NAME "Player1"
#define PLAYER_LIVES 3
#define PLAYER_BASE_JUMP	-5
#define PLAYER_BASE_SPEED	5

/**
 * Initialize the player entity and variables thereof, also used to reset player.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void InitPlayer();

//Self explanatory
void DecrementPlayerLives();

extern entity_t *gPlayer;   /**< The player entity */
extern char *gPlayerName;  /**< The name of the player */

#endif