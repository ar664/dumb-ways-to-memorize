#ifndef __PARSE_LEVEL_H
#define __PARSE_LEVEL_H

#include "parseobject.h"
#include "entity.h"

#define LEVEL_BACKGROUND_STR	"Background"
#define LEVEL_HINT_STR			"Hint"
#define LEVEL_SPAWN_STR			"Spawn"
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

struct level_s
{
	char *mName;
	char *mHint;
	sprite_t *mBackground;
	vec2_t mSpawnPoint;
	// Later implement Size
};

extern level_t *gCurrentLevel;

int LoadLevel(object_t *level, char *g_str);
void DrawLevel();

#endif