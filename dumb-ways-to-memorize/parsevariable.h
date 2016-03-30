#ifndef __PARSE_VARIABLE_H
#define __PARSE_VARIABLE_H

#include "entity.h"
#include "parseobject.h"

extern char *AssignableVariableNames[];
extern char *OtherVariableNames[];

/** Defines the enum for variables in the entity structure */
typedef enum
{
	ENTITY_MEMBER_HAZARDS,  /**< An enum constant representing the entity member hazards */
	ENTITY_MEMBER_HEALTH,   /**< An enum constant representing the entity member health */
	ENTITY_MEMBER_COLLISION_TYPE,   /**< An enum constant representing the entity member collision type */
	ENTITY_MEMBER_ENTITY_STATE, /**< An enum constant representing the entity member entity state */
	ENTITY_MEMBER_SPRITES,  /**< An enum constant representing the entity member sprites  */
	ENTITY_MEMBER_ACCEL,	/**< An enum constant representing the entity member accel  */
	ENTITY_MEMBER_VELOCITY, /**< An enum constant representing the entity member velocity  */
	ENTITY_MEMBER_POSITION, /**< An enum constant representing the entity member position */
	ENTITY_MEMBER_MAX

}EntityMembers;

/** Defines the enum for graphics members in the entity */
typedef enum
{
	HEIGHT, /**< An enum constant representing the heights of the sprites */
	WIDTH,  /**< An enum constant representing the widths of th sprites */
	FRAMES  /**< An enum constant representing the number of frames in each sprite */
}OtherNumbers;

void EditEntity(entity_t *ent, EntityMembers member, void *value);
void MiniParseFunc(entity_t *ent, jsmntok_t* token, char *str, EntityMembers member, int size);

extern entity_t *ParseToEntity(object_t *object, char* str);
extern vec2_t *ParseToVec2(object_t *object, char* str);
extern char **ParseToStringArray(object_t *object, char* str);


#endif