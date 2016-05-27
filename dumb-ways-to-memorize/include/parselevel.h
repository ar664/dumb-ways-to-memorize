#ifndef __PARSE_LEVEL_H
#define __PARSE_LEVEL_H

#include "parseobject.h"
#include "entity.h"

#define SAVE_FILE	"save.dat"

extern char *LevelGlobalObjectNames[];
extern char *LevelLocalObjectNames[];
extern char *LevelGlobalOptionNames[];
extern char *LevelLocalOptionNames[];

typedef enum
{
	LEVEL_G_OBJECT_ENEMIES,
	LEVEL_G_OBJECT_OBJECTS,
	LEVEL_G_OBJECT_SPAWN,
	LEVEL_G_OBJECT_PLAYLIST,
	LEVEL_G_OBJECT_MAX
}level_global_object_t;

typedef enum
{
	LEVEL_L_OBJECT_ENEMY,
	LEVEL_L_OBJECT_OBJECT,
	LEVEL_L_OBJECT_MAX
}level_local_object_t;

typedef enum
{
	LEVEL_G_OPTION_BACKGROUND,
	LEVEL_G_OPTION_HINT,
	LEVEL_G_OPTION_MAX
}level_global_option_t;

typedef enum
{
	LEVEL_L_OPTION_POSITION,
	LEVEL_L_OPTION_VARIABLES,
	LEVEL_L_OPTION_AI,
	LEVEL_L_OPTION_EXTRA,
	LEVEL_L_OPTION_TILE,
	LEVEL_L_OPTION_MAX
}level_local_option_t;

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
	struct sound_s *mCurrentSong; /**< The song currently playing */
	struct sound_s **mMusic;/**< The music playlist to be played for this level*/
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

int LoadGameState();

/**
 * Saves the level.
 *
 * @return	An int.
 *
 * @author	Anthony Rios
 * @date	4/29/2016
 */
int SaveGameState();

/**
 * Draws the background of the level, everything else is cool.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

void DrawLevel();

/**
 * Play the next music in the playlist of the current level.
 *
 * @author	Anthony Rios
 * @date	4/9/2016
 */

void LevelMusicNext();

/**
 * Spawns more versions of ent given start and end position , via a tiling square method.
 *
 * @param [in,out]	ent			  	If non-null, the ent.
 * @param [in,out]	start_position	If non-null, the start position.
 * @param [in,out]	end_position  	If non-null, the end position.
 *
 * @author	Anthony Rios
 * @date	4/9/2016
 */

void TileLevelEntity(entity_t *ent, vec2_t *start_position , vec2_t *end_position);

void AddGlobalObject(level_t *level ,object_t *obj, char *g_str, level_global_object_t type);
void AddLocalObject(level_t *level ,object_t *obj, char *g_str, level_local_object_t type);
void AddGlobalOption(level_t *level ,jsmntok_t *token, char *g_str, level_global_option_t type);
void AddLocalOption(entity_t *ent,void *token, char *g_str, level_local_option_t type);

#endif