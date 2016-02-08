#ifndef __PARSE_VARIABLE_H
#define __PARSE_VARIABLE_H

#include "entity.h"
#include "parseobject.h"

char *AssignableVariableNames[] = {"hazard(s)" , "collisionType", "entityState", "sprite(s)", "accel", "velocity", "position", 0};
char *OtherVariableNames[] = { "height", "width", "frames", 0 };

typedef enum
{
	HAZARDS,
	COLLISION_TYPE,
	ENTITY_STATE,
	SPRITES,
	ACCEL,
	VELOCITY,
	POSITION

}EntityNumbers;

entity_t *ParseToEntity(object_t *object);


#endif