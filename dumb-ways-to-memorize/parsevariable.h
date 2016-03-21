#ifndef __PARSE_VARIABLE_H
#define __PARSE_VARIABLE_H

#include "entity.h"
#include "parseobject.h"

extern char *AssignableVariableNames[];
extern char *OtherVariableNames[];

typedef enum
{
	ENTITY_MEMBER_HAZARDS,
	ENTITY_MEMBER_HEALTH,
	ENTITY_MEMBER_COLLISION_TYPE,
	ENTITY_MEMBER_ENTITY_STATE,
	ENTITY_MEMBER_SPRITES,
	ENTITY_MEMBER_ACCEL,
	ENTITY_MEMBER_VELOCITY,
	ENTITY_MEMBER_POSITION,
	ENTITY_MEMBER_MAX

}EntityMembers;

typedef enum
{
	HEIGHT,
	WIDTH,
	FRAMES
}OtherNumbers;

void EditEntity(entity_t *ent, EntityMembers member, void *value);
void MiniParseFunc(entity_t *ent, jsmntok_t* token, char *str, EntityMembers member, int size);

extern entity_t *ParseToEntity(object_t *object, char* str);
extern vec2_t *ParseToVec2(object_t *object, char* str);
extern char **ParseToStringArray(object_t *object, char* str);
extern void PrintEntity(entity_t *ent);

#endif