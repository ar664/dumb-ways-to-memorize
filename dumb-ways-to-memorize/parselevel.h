#ifndef __PARSE_LEVEL_H
#define __PARSE_LEVEL_H

#include "parseobject.h"
#include "entity.h"

#define LEVEL_BACKGROUND_STR	"Background"
#define LEVEL_HINT_STR			"Hint"
#define LEVEL_SPAWN_STR			"Spawn"
#define LEVEL_TILE_X_STR		"tile-to-x"
#define LEVEL_TILE_Y_STR		"tile-to-y"
#define LEVEL_ENEMY_OBJ_STR		"Enemies"
#define LEVEL_ENEMY_NAME_STR	"enemy"
#define LEVEL_ITEM_OBJ_STR		"Objects"
#define LEVEL_ITEM_NAME_STR		"object"
#define LEVEL_ITEM_XTRA_STR		"extra"
#define LEVEL_AI_STR			"ai"
#define LEVEL_POSITION_STR		"position"
#define LEVEL_FLAG_TILE_X_STR	"tile-to-pos-x"
#define LEVEL_FLAG_TILE_Y_STR	"tile-to-pos-y"
#define LEVEL_VARIABLES_STR		"variables"

typedef struct level_s level_t;

/**
 * The structure for the levels.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

struct level_s
{
	char *mName;			/**< The name of the level */
	char *mHint;			/**< The hint to be given to player if they need it */
	sprite_t *mBackground;  /**< The background image for the level */
	vec2_t mSpawnPoint;		/**< The spawn point of the player during init load */
	// Later implement Size
};

extern level_t *gCurrentLevel;

int LoadLevel(object_t *level, char *g_str);
void DrawLevel();

#endif