#ifndef __PARSE_LEVEL_H
#define __PARSE_LEVEL_H

#include "parseobject.h"
#include "entity.h"

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