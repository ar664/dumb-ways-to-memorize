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

/**
 * Directly Edit the entity with the given data and member type.
 *
 * @param [in,out]	ent  	If non-null, the ent.
 * @param	member		 	The member.
 * @param [in,out]	value	If non-null, the value.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void EditEntity(entity_t *ent, EntityMembers member, void *value);

/**
 * Mini parse function, handles entering / converting the data into the entity given.
 *
 * @param [in,out]	ent  	If non-null, the ent.
 * @param [in,out]	token	If non-null, the token.
 * @param [in,out]	str  	If non-null, the string.
 * @param	member		 	The member.
 * @param	size		 	The size.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void MiniParseFunc(entity_t *ent, jsmntok_t* token, char *str, EntityMembers member, int size);

/**
 * Parse to entity.
 * Tries to Assign everything , but the entity function pointers.
 * Current Frame = 0, Weight = 1
 *
 * @param [in,out]	object	If non-null, the object.
 * @param [in,out]	str   	If non-null, the string.
 *
 * @return	null if it fails, else a pointer to an entity_t.
 *
 * @author	Anthony Rios
 * @date	3/21/2016
 */
extern entity_t *ParseToEntity(object_t *object, char* str);

/**
 * Parse given obj/str to vector 2.
 *
 * @param [in,out]	object	If non-null, the object.
 * @param [in,out]	str   	If non-null, the string.
 *
 * @return	null if it fails, else a pointer to a vec2_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern vec2_t *ParseToVec2(object_t *object, char* str);

/**
 * Parse obj/data to string array.
 *
 * @param [in,out]	object	If non-null, the object.
 * @param [in,out]	str   	If non-null, the string.
 *
 * @return	null if it fails, else a handle to a char.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern char **ParseToStringArray(object_t *object, char* str);


#endif