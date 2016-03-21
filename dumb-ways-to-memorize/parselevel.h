#ifndef __PARSE_LEVEL_H
#define __PARSE_LEVEL_H

#include "parseobject.h"
#include "entity.h"

#define LEVEL_BACKGROUND_STR	"Background"
#define LEVEL_HINT_STR			"Hint"
#define LEVEL_SPAWN_STR			"Spawn"
#define LEVEL_ENEMY_OBJ_STR		"Enemy"
#define LEVEL_ENEMY_NAME_STR	"enemy"
#define LEVEL_ITEM_OBJ_STR		"Objects"
#define LEVEL_ITEM_NAME_STR		"object"
#define LEVEL_AI_STR			"ai"
#define LEVEL_POSITION_STR		"position"
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