#ifndef __PARSE_LEVEL_H
#define __PARSE_LEVEL_H

#include "parseobject.h"
#include "entity.h"

extern char *LevelObjectNames[];
extern char *LevelItemNames[];
extern char *LevelOptionNames[];

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
	struct sound_s *mMusic;	/**< The music to be played for this level*/
	vec2_t mSpawnPoint;		/**< The spawn point of the player during init load */
	// Later implement Size
};

extern level_t *gCurrentLevel; /**< The current level the game is on */

/**
 * Loads a level.
 *
 * @param [in,out]	level	If non-null, the level object.
 * @param [in,out]	g_str	If non-null, the level string data for obj reference.
 *
 * @return	0 if successful, -1 if not successful.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

int LoadLevel(object_t *level, char *g_str);

/**
 * Draws the background of the level, everything else is cool.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

void DrawLevel();

#endif