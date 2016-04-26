#ifndef __PLAYER_H
#define __PLAYER_H

#include "entity.h"
#define PLAYER_NAME "Player1"
#define PLAYER_LIVES 3
#define PLAYER_BASE_JUMP	-100
#define PLAYER_BASE_SPEED	15

/**
 * Initialize the player entity and variables thereof, also used to reset player.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void InitPlayer();
void InitCursor();

#define PLAYER_STR "Player"
#define CURSOR_STR "Cursor"

extern entity_t *gPlayer;   /**< The player entity */
extern entity_t *gCursor;   /**< The cursor entity corresponding the the sprite of it */
extern char *gPlayerName;  /**< The name of the player */

#endif