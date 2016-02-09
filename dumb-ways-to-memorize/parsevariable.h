#ifndef __PARSE_VARIABLE_H
#define __PARSE_VARIABLE_H

#include "entity.h"
#include "parseobject.h"

extern char *AssignableVariableNames[];
extern char *OtherVariableNames[];

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

void EditEntity(entity_t *ent, EntityNumbers member, void *value);

entity_t *ParseToEntity(object_t *object, char* str);
vec2_t *ParseToVec2(object_t *object, char* str);
char **ParseToStringArray(object_t *object, char* str);

#endif