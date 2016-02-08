#ifndef __PARSE_VARIABLE_H
#define __PARSE_VARIABLE_H

#include "entity.h"
#include <jsmn.h>

typedef enum
{
	COLLISION_TYPE,
	ENTITY_STATE,
	SPRITES,
	ACCEL,
	VELOCITY,
	POSITION

}EntityNumbers;

void Variable(jsmntok_t *key, jsmntok_t *value);


#endif