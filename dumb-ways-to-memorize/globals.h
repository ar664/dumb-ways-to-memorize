#ifndef __GLOBALS_H
#define __GLOBALS_H

#define	FRAME_DELAY		17

extern int exitRequest;

enum CollisionTypes
{
	COLLISION_TYPE_STATIC,
	COLLISION_TYPE_DYNAMIC,
	COLLISION_TYPE_SPECIAL
};

//JSON Parser
#include <jsmn.h>
extern jsmn_parser gParser;
extern jsmntok_t *gTokens;

#endif